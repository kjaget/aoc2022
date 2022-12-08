
def i_win_game(their_move, my_move):
    winning_moves = {'A' : 'B', 'B' : 'C', 'C' : 'A'}
    return winning_moves[their_move] == my_move

# A = rock
# B = paper
# C = scissors
def evaluate_game(their_move, my_move):
    score = ord(my_move) - ord('A') + 1
    if my_move == their_move:
        score += 3
    elif i_win_game(their_move, my_move):
        score += 6

    return score


def xyz_to_abc(xyz_input):
    dict = {'X':'A', 'Y':'B', 'Z':'C'}

    return dict[xyz_input]


# X = lose
# Y = draw
# Z = win
def pick_my_move(their_move, goal):
    tying_moves = {'A' : 'A', 'B': 'B', 'C': 'C'}
    losing_moves = {'A' : 'C', 'B' : 'A', 'C' : 'B'}
    winning_moves = {'A' : 'B', 'B' : 'C', 'C' : 'A'}

    moves = {'X' : losing_moves, 'Y' : tying_moves, 'Z' : winning_moves}
    return moves[goal][their_move]

def main():
    '''
    print(f'{evaluate_game("A",xyz_to_abc("Y"))}')
    print(f'{evaluate_game("B",xyz_to_abc("X"))}')
    print(f'{evaluate_game("C",xyz_to_abc("Z"))}')
    print(f'{evaluate_game("A",pick_my_move("A","Y"))}')
    print(f'{evaluate_game("B",pick_my_move("B","X"))}')
    print(f'{evaluate_game("C",pick_my_move("C","Z"))}')
    '''
    file1 = open('input.txt', 'r')

    score1 = 0
    score2 = 0
    for line in file1:
        score1 += evaluate_game(line[0], xyz_to_abc(line[2]))
        score2 += evaluate_game(line[0], pick_my_move(line[0], line[2]))

    # Closing files
    file1.close()

    print(f'Score 1 = {score1}')
    print(f'Score 2 = {score2}')


if __name__ == "__main__":
    main()