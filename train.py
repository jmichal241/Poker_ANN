import torch
import torch.nn as nn
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
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = self.out(x)
        return x

# === Funkcja trenująca ===
def train_model(train_dataset, valid_dataset, model, epochs=100, batch_size=32, learning_rate=0.001, device=torch.device('cuda' if torch.cuda.is_available() else 'cpu')):
    model.to(device)
    train_loader = DataLoader(train_dataset, batch_size=batch_size, shuffle=True, num_workers=4, pin_memory=True)
    valid_loader = DataLoader(valid_dataset, batch_size=batch_size, shuffle=False, num_workers=4, pin_memory=True)

    criterion = nn.CrossEntropyLoss()
    optimizer = optim.Adam(model.parameters(), lr=learning_rate)

    for epoch in range(epochs):
        model.train()
        total_loss = 0.0
        for inputs, labels in train_loader:
            inputs, labels = inputs.to(device, non_blocking=True), labels.to(device, non_blocking=True)
            outputs = model(inputs)
            loss = criterion(outputs, labels)

            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
            total_loss += loss.item()

        # Walidacja po każdej epoce
        model.eval()
        correct = 0
        total = 0
        with torch.no_grad():
            for inputs, labels in valid_loader:
                inputs, labels = inputs.to(device), labels.to(device)
                outputs = model(inputs)
                predictions = torch.argmax(outputs, dim=1)
                correct += (predictions == labels).sum().item()
                total += labels.size(0)

        accuracy = correct / total * 100 if total > 0 else 0
        print(f"Epoch {epoch+1}/{epochs}, Loss: {total_loss:.4f}, Validation Accuracy: {accuracy:.2f}%")

# === Funkcja testująca ===
def test_model(model, test_dataset, device=torch.device('cuda' if torch.cuda.is_available() else 'cpu'), num_samples=10):
    model.eval()
    model.to(device)

    correct = 0
    with torch.no_grad():
        for i in range(min(num_samples, len(test_dataset))):
            x, label = test_dataset[i]
            x = x.to(device).unsqueeze(0)
            output = model(x)
            predicted = torch.argmax(output, dim=1).item()
            print(f"Sample {i+1}: Predicted = {predicted}, Actual = {label.item()}")
            if predicted == label.item():
                correct += 1
    print(f"Accuracy on {num_samples} samples: {correct}/{num_samples}")

# === Główna sekcja ===
if __name__ == "__main__":
    train_dataset = PokerDataset("dataset/train")
    valid_dataset = PokerDataset("dataset/valid")
    test_dataset = PokerDataset("dataset/test")

    print(f"Train: {len(train_dataset)} hands | Valid: {len(valid_dataset)} | Test: {len(test_dataset)}")

    input_size = len(train_dataset[0][0])
    model = PokerNet(input_size=input_size)

    train_model(train_dataset, valid_dataset, model, epochs=50, batch_size=64)
    test_model(model, test_dataset, num_samples=20)
