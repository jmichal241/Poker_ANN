import torch
import torch.nn as nn
import torch.nn.functional as F
import torch.optim as optim
from torch.utils.data import DataLoader
from prepare_data import PokerDataset


# === Model sieci neuronowej ===
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

def forward(self, x):
    # Create a mask to ignore padding (value -1)
    mask = (x != -1).float()  # Mask will be 1 where it's not padding, 0 where it's padding

    x = torch.relu(self.fc1(x))
    x = torch.relu(self.fc2(x))

    # Multiply by the mask to ignore padding during the final layer
    x = x * mask[:, :x.size(1)]  # Ensure the mask matches the size of the output

    x = self.fc3(x)
    return x
# === Funkcja trenująca ===
def train_model(dataset, model, epochs=1000, batch_size=32, learning_rate=0.001):
    dataloader = DataLoader(dataset, batch_size=batch_size, shuffle=True)

    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate)

    for epoch in range(epochs):
        total_loss = 0.0
        for inputs, labels in dataloader:
            outputs = model(inputs)
            loss = criterion(outputs, labels)

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()

            total_loss += loss.item()

        print(f"Epoch {epoch + 1}/{epochs}, Loss: {total_loss:.4f}")


# === Główna sekcja ===
if __name__ == "__main__":
    dataset = PokerDataset("dataset")
    print(f"Loaded {len(dataset)} hands.")

    input_size = len(dataset[0][0])
    model = PokerNet(input_size=input_size)

    train_model(dataset, model, epochs=1000)
