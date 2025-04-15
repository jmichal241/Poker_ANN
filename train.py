import torch
import torch.nn as nn
import torch.optim as optim
import numpy as np
from rnn import PokerRNN

# Parametry
input_size = 30
hidden_size = 128
output_size = 3
seq_length = 5
num_layers = 1
learning_rate = 0.001
num_epochs = 500
batch_size = 32

# Dummy dane
def generate_dummy_data(num_samples=500):
    X = np.random.rand(num_samples, seq_length, input_size).astype(np.float32)
    y = np.random.randint(0, output_size, size=(num_samples,))
    return torch.tensor(X), torch.tensor(y)

X, y = generate_dummy_data()

dataset = torch.utils.data.TensorDataset(X, y)
loader = torch.utils.data.DataLoader(dataset, batch_size=batch_size, shuffle=True)

device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
model = PokerRNN(input_size, hidden_size, output_size, num_layers).to(device)
criterion = nn.CrossEntropyLoss()
optimizer = optim.Adam(model.parameters(), lr=learning_rate)

for epoch in range(num_epochs):
    total_loss = 0
    for batch_X, batch_y in loader:
        batch_X, batch_y = batch_X.to(device), batch_y.to(device)

        outputs = model(batch_X)
        loss = criterion(outputs, batch_y)

        optimizer.zero_grad()
        loss.backward()
        optimizer.step()

        total_loss += loss.item()

    print(f"Epoch [{epoch+1}/{num_epochs}], Loss: {total_loss:.4f}")

# Testowanie modelu po treningu
model.eval()  # Przełącza model w tryb ewaluacji

# Przykładowe dane do testowania
test_data, test_labels = generate_dummy_data(5)  # 5 próbek testowych
test_data, test_labels = test_data.to(device), test_labels.to(device)

with torch.no_grad():  # Nie musimy śledzić gradientów przy testowaniu
    predictions = model(test_data)

# Drukowanie wyników
for i in range(len(predictions)):
    print(f"Przykład {i+1}:")
    print(f"  Prawdziwa etykieta: {test_labels[i].item()}")
    print(f"  Predykcja: {predictions[i].argmax().item()}")
