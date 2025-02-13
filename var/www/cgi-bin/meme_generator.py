#!/usr/bin/env python3
import os
import random

# Récupérer la variable d'environnement WORD
word = os.getenv("WORD", "MISSING")  # Si "WORD" n'est pas défini, utiliser "MISSING"
word = word.strip()  # Nettoyer les espaces superflus

# Liste de phrases avec le mot inséré dedans
phrases = [
    "{} le chat est devenu connu en sautant du 10ème étage d'un bâtiment en feu. Il en est ressorti indemne et est surnommé maintenant Fire Cat !",
    "Le vrai nom de Nyan Cat est en réalité {} !",
    "{} alias \"Garfield\" est en réalité juste un gros chat orange connu pour être le plus gros chat orange de la planète. Et il aime les lasagnes.",
    "Vous connaissez Bath's {} ? C'est un chat connu pour adorer prendre des bains.",
    "Grumpy {} est devenu célèbre pour faire la gueule tout le temps. Malheureusement, il est mort.",
    "{} le chat a simplement été élu le plus beau chat du monde en 2023 !",
    "{} est un magnifique chat gracieux connu pour avoir la queue la plus fluffy d'Internet !",
    "Dans la ville de Goupiland, {} a été élu Maire de la Ville. Il est en poste depuis 2012."
]


# Générer une phrase aléatoire
result = random.choice(phrases).format(word)

# Répondre en CGI
print(result)
