import torch

class AddModule(torch.nn.Module):
    def __init__(self):
        super().__init__()

    def forward(self, x):
        return x + x

    example_input = (torch.ones(2, 2),)
    can_delegate = True 