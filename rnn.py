import torch
import torch.nn as nn
import torch.nn.functional as F

class PokerNet(nn.Module):
    def __init__(self, input_size, hidden_size=64, output_size=4):
        super(PokerNet, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.fc2 = nn.Linear(hidden_size, hidden_size)
        self.out = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        return self.out(x)
