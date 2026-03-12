import torch
import torch.nn as nn
import sys

# ------------------------------------------------------------------ #
#  Must match the architecture in train.py exactly                    #
# ------------------------------------------------------------------ #
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

# ------------------------------------------------------------------ #
#  Read number from command line argument                             #
# ------------------------------------------------------------------ #
if len(sys.argv) < 2:
    print("usage: python predict.py <number>")
    sys.exit(1)

n = int(sys.argv[1])

# One-hot encode the last digit — must match training
last_digit = n % 10
X = torch.zeros(1, 10)
X[0][last_digit] = 1.0

# ------------------------------------------------------------------ #
#  Predict                                                            #
# ------------------------------------------------------------------ #
with torch.no_grad():
    output = model(X).item()

# >0.5 = odd (label 1), <=0.5 = even (label 0)
print("false" if output > 0.5 else "true")