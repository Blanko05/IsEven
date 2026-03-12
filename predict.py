import torch
import torch.nn as nn
import sys

model = nn.Sequential(
    nn.Linear(10, 16),
    nn.ReLU(),
    nn.Linear(16, 16),
    nn.ReLU(),
    nn.Linear(16, 1),
    nn.Sigmoid()
)

model.load_state_dict(torch.load("model.pth", weights_only=True))
model.eval()


if len(sys.argv) < 2:
    print("usage: python predict.py <number>")
    sys.exit(1)

n = int(sys.argv[1])


last_digit = n % 10
X = torch.zeros(1, 10)
X[0][last_digit] = 1.0


with torch.no_grad():
    output = model(X).item()


print("false" if output > 0.5 else "true")
