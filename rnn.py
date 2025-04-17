import torch
import torch.nn as nn
import torch.nn.functional as F

class PokerNet(nn.Module):
    def __init__(self, input_size, hidden_size=128, output_size=4):
        super(PokerNet, self).__init__()
        self.fc1 = nn.Linear(input_size, hidden_size)
        self.bn1 = nn.BatchNorm1d(hidden_size)
        self.fc2 = nn.Linear(hidden_size, hidden_size)
        self.bn2 = nn.BatchNorm1d(hidden_size)
        self.fc3 = nn.Linear(hidden_size, hidden_size)
        self.dropout = nn.Dropout(0.5)
        self.out = nn.Linear(hidden_size, output_size)

    def forward(self, x):
        x = F.relu(self.bn1(self.fc1(x)))
        x = self.dropout(x)
        x = F.relu(self.bn2(self.fc2(x)))
        x = self.dropout(x)
        x = F.relu(self.fc3(x))
        x = self.out(x)
        return x

# Ustawienie urządzenia (GPU lub CPU)
device = torch.device("cuda" if torch.cuda.is_available() else "cpu")

# Inicjalizacja modelu
input_size = 100  # Dopasuj to do rozmiaru danych wejściowych
model = PokerNet(input_size).to(device)  # Przenosimy model na GPU/CPU

# Sprawdzenie, czy model jest poprawnie na GPU
print(f"Model is running on: {device}")
