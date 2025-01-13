import torch
from torch import nn
from torch.export import export, export_for_training, ExportedProgram
from executorch.exir import ExecutorchBackendConfig, ExecutorchProgramManager
import executorch.exir as exir
import pytorch_lightning as pl
from torch.utils.data import DataLoader, TensorDataset

# Define the Conv1D Model with LightningModule
class Conv1DModel(pl.LightningModule):
    def __init__(self, input_channels, output_channels, kernel_size):
        super().__init__()
        self.conv1d = nn.Conv1d(input_channels, output_channels, kernel_size)

    def forward(self, x):
        # Ensure input is [batch_size, input_channels, seq_length]
        return self.conv1d(x)

    def training_step(self, batch, batch_idx):
        x, y = batch
        output = self(x)
        loss = nn.functional.mse_loss(output[:, :, -1], y)  # Example loss
        self.log("train_loss", loss)
        return loss

    def configure_optimizers(self):
        return torch.optim.SGD(self.parameters(), lr=0.01)

# Prepare Dummy Data
x_data = torch.randn(10, 16, 10)  # [batch_size, input_channels, seq_length]
y_data = torch.randn(10, 15)  # [batch_size, output_channels]
dataset = TensorDataset(x_data, y_data)
train_loader = DataLoader(dataset, batch_size=2)

# Train the Model
model = Conv1DModel(input_channels=16, output_channels=15, kernel_size=10)
trainer = pl.Trainer(max_epochs=5, logger=False)
trainer.fit(model, train_loader)

# Export the Model
model.eval()

# Example Input
example_input = torch.randn(1, 16, 10)  # [batch_size, input_channels, seq_length]

# Export the Model
pre_autograd_aten_dialect = export_for_training(
        model,
        (example_input,)
    ).module()

aten_dialect: ExportedProgram = export(pre_autograd_aten_dialect, (example_input,))
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

