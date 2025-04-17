import os
import json
import torch
from torch.utils.data import Dataset

SUIT_MAP = {"Spade": 0, "Clubs": 1, "Diamond": 2, "Heart": 3}
ACTION_MAP = {"pass": 0, "call": 1, "check": 2, "raise": 3}

class PokerDataset(Dataset):
    def __init__(self, folder_paths, max_actions=15):
        self.data = []
        self.labels = []
        self.max_actions = max_actions

        # Pozwala na podanie jednego folderu albo listy folderów
        if isinstance(folder_paths, str):
            folder_paths = [folder_paths]

        for folder_path in folder_paths:
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
                            x.append(card["card"])
                            x.append(SUIT_MAP[card["suit"]])

                        # Actions
                        actions = game["actions"]
                        x.append(len(actions))  # liczba akcji

                        for action in actions:
                            x.append(action["player"])
                            x.append(ACTION_MAP[action["action"]])
                            x.append(action.get("amount", 0) if action["action"] == "raise" else 0)

                        # Padding
                        for _ in range(self.max_actions - len(actions)):
                            x.extend([-1, -1, -1])

                        # Winner encoding
                        winner = game["winner"]
                        if isinstance(winner, list):
                            x.append(len(winner))
                            for w in winner:
                                x.append(w)
                            for _ in range(6 - len(winner)):
                                x.append(-1)
                        else:
                            x.append(1)
                            x.append(winner)
                            for _ in range(5):
                                x.append(-1)

                        x.append(game["pot"])

                        self.data.append(torch.tensor(x, dtype=torch.float))

                        # Label = pierwsza akcja bohatera
                        hero_action = None
                        for action in game["actions"]:
                            if action["player"] == hero_info["player"]:
                                hero_action = action
                                break

                        if hero_action is None:
                            print(f"No action found for hero {hero_info['player']} in game {filename}. Skipping this game.")
                            continue

                        self.labels.append(torch.tensor(ACTION_MAP[hero_action["action"]], dtype=torch.long))

        print(f"[PokerDataset] Loaded {len(self.data)} hands from: {folder_paths}")

    def __len__(self):
        return len(self.data)

    def __getitem__(self, idx):
        return self.data[idx], self.labels[idx]

    def get_player_hand(self, idx):
        game_data = self.data[idx]
        return int(game_data[3])  # Hero player index


# Test
if __name__ == "__main__":
    dataset = PokerDataset(["dataset/train", "dataset/valid", "dataset/test"])
    print(f"Loaded {len(dataset)} hands.")
    print("Sample input:", dataset[0][0])
    print("Sample label:", dataset[0][1])
    print(f"The sample hand belongs to player: {dataset.get_player_hand(0)}")
