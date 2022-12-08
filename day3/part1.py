import string

def get_items(list):
    return {c for c in list}

def item_val(item_char):
    item_num = ord(item_char)
    if item_char in string.ascii_lowercase:
        return item_num - ord('a') + 1

    return item_num - ord('A') + 27

def get_line_priority(line):
    half_len = len(line) // 2
    c = get_items(line[:half_len]).intersection(get_items(line[half_len:])).pop()
    return item_val(c)


def main():
    file1 = open('input.txt', 'r')

    sum = 0
    for line in file1:
        sum += get_line_priority(line.rstrip('\n'))

    print(f'Sum = {sum}')


if __name__ == "__main__":
    main()