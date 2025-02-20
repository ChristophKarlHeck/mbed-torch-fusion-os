import torch
from torch import nn
import torch.nn.functional as F
from torch.export import export, export_for_training, ExportedProgram
from executorch.exir import ExecutorchBackendConfig, ExecutorchProgramManager
import executorch.exir as exir
import pytorch_lightning as pl
from torch.utils.data import DataLoader, TensorDataset

import numpy as np

# Define the Conv1D Model with LightningModule
class Conv1DModel(pl.LightningModule):
    def __init__(self, input_channels, output_channels, kernel_size):
        super().__init__()
        self.conv1d = nn.Conv1d(input_channels, output_channels, kernel_size)
        self.pool = nn.MaxPool1d(3, stride=3) # window size 3, how far windows slided 3
        self.linear = nn.Linear(64, 16) # fully connected layer
        self.output = nn.Linear(16, 2)
        self.loss_fn = nn.CrossEntropyLoss()
        # pooling

    def forward(self, x):
        # Ensure input is [batch_size, input_channels, seq_length]
        x = self.conv1d(x)
        x = F.relu(x)
        x = self.pool(x) # compress to one convolution block
        x = torch.transpose(x, 1, 2)
        x = torch.cat((x[:,:,0],x[:,:,1]), dim=1)
        x = self.linear(x)
        x = F.relu(x) # ReLu is not linear. At least one non-linear to recognize non-linear pattern
        x = F.softmax(self.output(x),dim=1) # Sum = 1
        return x

    def training_step(self, batch, batch_idx):
        x, y = batch
        output = self(x)
        print(output.shape)
        y = torch.argmax(y, dim=1)
        loss = self.loss_fn(output, y)  # Example loss
        self.log("train_loss", loss)
        return loss

    def configure_optimizers(self):
        return torch.optim.SGD(self.parameters(), lr=0.01)
    
    example_input = torch.randn(1, 1, 100)
    can_delegate = False

# Prepare Dummy Data
x_data = torch.randn(10, 1, 100)  # [batch_size, input_channels, seq_length[50,100]] double check if input channels and seq_length are switched (lstm opposite)
y_data = torch.randn(10, 1)  # [batch_size, output_channels]
dataset = TensorDataset(x_data, y_data)
train_loader = DataLoader(dataset, batch_size=3)

# Train the Model
model = Conv1DModel(input_channels=1, output_channels=2, kernel_size=5)
trainer = pl.Trainer(max_epochs=5, logger=False)
trainer.fit(model, train_loader)

# Export the Model
model.eval()

# Example Input
final_example_input = (torch.randn(1, 1, 100),)  # [batch_size, input_channels, seq_length]

# Export the Model
pre_autograd_aten_dialect = export_for_training(
        model,
        final_example_input
    ).module()

aten_dialect: ExportedProgram = export(pre_autograd_aten_dialect, final_example_input)

# The graph returned by torch.export only contains functional ATen operators (~2000 ops), which we will call the ATen Dialect.
print(aten_dialect)
edge_program: exir.EdgeProgramManager = exir.to_edge(aten_dialect)

executorch_program: exir.ExecutorchProgramManager = edge_program.to_executorch(
    ExecutorchBackendConfig(
        passes=[],  # User-defined passes
    )
)

with open("model.pte", "wb") as file:
    file.write(executorch_program.buffer)

print("CNN model saved as model.pte")