import os
import json
import torch
from torch.utils.data import Dataset

SUIT_MAP = {"Spade": 0, "Clubs": 1, "Diamond": 2, "Heart": 3}
ACTION_MAP = {"pass": 0, "call": 1, "check": 2, "raise": 3}

class PokerDataset(Dataset):
    def __init__(self, folder_path):
        self.data = []
        self.labels = []

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

                    # Previous actions (padding if needed)
                    actions = game["actions"]
                    max_actions = 10  # Możesz ustawić dowolny limit na liczbę akcji, np. 10

                    # Dodajemy wszystkie akcje do x
                    for action in actions[:max_actions]:
                        x.append(action["player"])
                        x.append(ACTION_MAP[action["action"]])

                    # Padding, jeśli akcji jest mniej niż max_actions
                    for _ in range(max_actions - len(actions)):
                        x.extend([0, 0])  # [0, 0] dla gracza i akcji


                    self.data.append(torch.tensor(x, dtype=torch.float))

                    # Extract hero action as label
                    hero_action = next(a for a in game["actions"] if a["player"] == hero_info["player"])
                    self.labels.append(torch.tensor(ACTION_MAP[hero_action["action"]], dtype=torch.long))

    def __len__(self):
        return len(self.data)

    def __getitem__(self, idx):
        return self.data[idx], self.labels[idx]

    def get_player_hand(self, idx):
        """
        Funkcja zwraca numer gracza (ręki) dla danego `sample`.
        """
        # Odzyskujemy dane z pliku, na podstawie indeksu
        game_data = self.data[idx]
        # Gracz (player) jest zapisany w pierwszym elemencie (po Hero_info), 
        # gdzie zapisujemy numer gracza.
        player_idx = int(game_data[3])  # To jest 'player' z Hero_info
        return player_idx


# Example usage
if __name__ == "__main__":
    dataset = PokerDataset("dataset")
    print(f"Loaded {len(dataset)} hands.")
    print("Sample input:", dataset[0][0])
    print("Sample label:", dataset[0][1])

    # Sprawdzanie, która ręka jest przypisana do danego sample
    hand_player = dataset.get_player_hand(0)  # Sprawdzamy rękę dla pierwszego sample
    print(f"The sample hand belongs to player: {hand_player}")
