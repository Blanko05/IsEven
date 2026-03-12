import torch
import torch.nn as nn
import pandas as pd
from torch.utils.data import DataLoader, TensorDataset

# ------------------------------------------------------------------ #
#  Load data                                                           #
# ------------------------------------------------------------------ #
df = pd.read_csv("training_data.csv")

last_digits = df["number"].values % 10
X = torch.zeros(len(last_digits), 10)
for i, d in enumerate(last_digits):
    X[i][d] = 1.0

y = torch.tensor(df["label"].values,  dtype=torch.float32).unsqueeze(1)


dataset = TensorDataset(X, y)
loader  = DataLoader(dataset, batch_size=32, shuffle=True)

# ------------------------------------------------------------------ #
#  Define the network                                                  #
# ------------------------------------------------------------------ #
# Input:        1 neuron  (the number)
# Hidden layer: 16 neurons, ReLU activation
# Hidden layer: 16 neurons, ReLU activation
# Output:       1 neuron, Sigmoid (outputs 0.0-1.0, >0.5 = odd)

model = nn.Sequential(
    nn.Linear(10, 16),
    nn.ReLU(),
    nn.Linear(16, 16),
    nn.ReLU(),
    nn.Linear(16, 1),
    nn.Sigmoid()
)

# ------------------------------------------------------------------ #
#  Train                                                               #
# ------------------------------------------------------------------ #
loss_fn   = nn.BCELoss()        # Binary Cross Entropy — standard for yes/no problems
optimizer = torch.optim.Adam(model.parameters(), lr=0.001)

EPOCHS = 100

for epoch in range(EPOCHS):
    total_loss = 0
    for batch_X, batch_y in loader:
        optimizer.zero_grad()           # reset gradients
        predictions = model(batch_X)    # forward pass
        loss = loss_fn(predictions, batch_y)
        loss.backward()                 # backward pass — adjust weights
        optimizer.step()
        total_loss += loss.item()

    if (epoch + 1) % 10 == 0:
        print(f"Epoch {epoch + 1}/{EPOCHS}  loss: {total_loss / len(loader):.4f}")

# ------------------------------------------------------------------ #
#  Evaluate accuracy                                                   #
# ------------------------------------------------------------------ #
with torch.no_grad():
    preds = model(X)
    predicted_labels = (preds > 0.5).float()
    accuracy = (predicted_labels == y).float().mean()
    print(f"\nTraining accuracy: {accuracy.item() * 100:.2f}%")

# ------------------------------------------------------------------ #
#  Save                                                                #
# ------------------------------------------------------------------ #
torch.save(model.state_dict(), "model.pth")
print("Model saved to model.pth")