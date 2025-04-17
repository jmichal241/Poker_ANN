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
        # Apply first layer
        x = torch.relu(self.fc1(x))

        # Apply second layer
        x = torch.relu(self.fc2(x))

        # Apply the final output layer
        x = self.out(x)
        return x




# === Funkcja trenująca ===
def train_model(dataset, model, epochs=1000, batch_size=32, learning_rate=0.001, device=torch.device('cuda' if torch.cuda.is_available() else 'cpu')):
    # Move model to the device (GPU/CPU)
    model.to(device)
    
    # Use DataLoader with more workers to parallelize data loading
    dataloader = DataLoader(dataset, batch_size=batch_size, shuffle=True, num_workers=4, pin_memory=True)

    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate)

    for epoch in range(epochs):
        total_loss = 0.0
        for inputs, labels in dataloader:
            # Move data to the device (GPU/CPU)
            inputs, labels = inputs.to(device, non_blocking=True), labels.to(device, non_blocking=True)

            # Forward pass
            outputs = model(inputs)
            loss = criterion(outputs, labels)

            # Backward pass and optimization
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

    train_model(dataset, model, epochs=1000, batch_size=64)

