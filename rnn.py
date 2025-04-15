import torch
import torch.nn as nn

class PokerRNN(nn.Module):
    def __init__(self, input_size, hidden_size, output_size, num_layers=1):
        super(PokerRNN, self).__init__()
        self.rnn = nn.RNN(input_size, hidden_size, num_layers, batch_first=True)
        self.fc = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        out, _ = self.rnn(x)
        out = self.fc(out[:, -1, :])  # bierzemy tylko ostatni timestep
        return out
