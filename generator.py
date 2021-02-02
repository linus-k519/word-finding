import random

# Read duden textfile
duden = []
with open('duden.txt') as file:
    duden = file.readlines()
    
# Remove newlines
for index, line in enumerate(duden):
    duden[index] = line[:-1]

# Filter for words of given length
word_len = int(input('Word length: '))
duden = list(filter(lambda x: len(x) == word_len, duden))

num_words = int(input('Number of words: '))

for i in range(num_words):
    # Select random element
    random_number = random.randint(0, len(duden))
    word = duden[random_number].upper()

    # Disorder word
    disordered_word = ''.join(random.sample(word, len(word)))
    print(disordered_word, ' -> ', word)
