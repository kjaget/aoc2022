import string

def get_items(list):
    return {c for c in list}

def item_val(item_char):
    item_num = ord(item_char)
    if item_char in string.ascii_lowercase:
        return item_num - ord('a') + 1

    return item_num - ord('A') + 27


def get_badge_priority(lines):
    sets = [get_items(l) for l in lines]
    s0 = set(string.ascii_letters)

    for s in sets:
        s0 = s0.intersection(s)

    c = s0.pop()
    return item_val(c)


def main():
    file = open('input.txt', 'r')

    sum = 0
    for line in file:
        lines = [line.rstrip('\n')]
        lines.append(next(file).rstrip('\n'))
        lines.append(next(file).rstrip('\n'))
        sum += get_badge_priority(lines)

    print(f'Sum = {sum}')


if __name__ == "__main__":
    main()