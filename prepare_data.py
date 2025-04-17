import os
import json
import torch
from torch.utils.data import Dataset

SUIT_MAP = {"Spade": 0, "Clubs": 1, "Diamond": 2, "Heart": 3}
ACTION_MAP = {"pass": 0, "call": 1, "check": 2, "raise": 3}

class PokerDataset(Dataset):
    def __init__(self, folder_path, max_actions=10):
        self.data = []
        self.labels = []
        self.max_actions = max_actions

        for filename in os.listdir(folder_path):
            if filename.endswith(".json"):
                with open(os.path.join(folder_path, filename), 'r') as f:
                    game = json.load(f)
                    
                    x = []

                    # Encode basic info
                    x.append(game["Number of players"])
                    x.append(game["Button on player"])
                    x.append(game["BB is"])

                    # Hero cards
                    hero_info = game["Hero_info"]
                    x.append(hero_info["player"])
                    for card in hero_info["cards"]:
                        x.append(card["card"])  # value (2-14)
                        x.append(SUIT_MAP[card["suit"]])  # suit (0-3)

                    # Count of real actions
                    actions = game["actions"]
                    x.append(len(actions))  # <-- NEW: real number of actions

                    # Add actual actions
                    for action in actions:
                        x.append(action["player"])  # Gracz wykonujący akcję
                        x.append(ACTION_MAP[action["action"]])  # Akcja gracza
                        x.append(action.get("amount", 0)) if action["action"] == "raise" else x.append(0)

                    # Padding for actions if necessary
                    for _ in range(self.max_actions - len(actions)):
                        x.extend([-1, -1, -1])  # padding

                    # Add result (winner and pot)
                    winner = game["winner"]
                    if isinstance(winner, list):
                        x.append(len(winner))  # liczba zwycięzców
                        for w in winner:
                            x.append(w)
                        # padding do 6 graczy (jeśli np. tylko 2 wygrało)
                        for _ in range(6 - len(winner)):
                            x.append(-1)
                    else:
                        x.append(1)  # jeden zwycięzca
                        x.append(winner)
                        for _ in range(5):  # padding żeby było zawsze 6 pozycji
                            x.append(-1)

                    x.append(game["pot"])

                    self.data.append(torch.tensor(x, dtype=torch.float))

                    # Label = pierwsza akcja bohatera
                    hero_action = next(a for a in game["actions"] if a["player"] == hero_info["player"])
                    self.labels.append(torch.tensor(ACTION_MAP[hero_action["action"]], dtype=torch.long))

    def __len__(self):
        return len(self.data)

    def __getitem__(self, idx):
        return self.data[idx], self.labels[idx]

    def get_player_hand(self, idx):
        game_data = self.data[idx]
        return int(game_data[3])  # player z Hero_info


if __name__ == "__main__":
    dataset = PokerDataset("dataset")
    print(f"Loaded {len(dataset)} hands.")
    print("Sample input:", dataset[0][0])
    print("Sample label:", dataset[0][1])
    hand_player = dataset.get_player_hand(0)
    print(f"The sample hand belongs to player: {hand_player}")
