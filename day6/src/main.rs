use std::fs;

use std::collections::HashSet;

fn test_index(data: &[u8], start_idx: usize, len: usize) -> bool {
    let mut unique_letters = HashSet::new();

    let mut counter = 0;
    for i in start_idx..start_idx + len {
        unique_letters.insert(data[i]);
        counter += 1;
        if counter != unique_letters.len() {
            return false;
        }
    }
    return true;
}

fn find_first_index(data: &[u8], len: usize) -> usize {
    for i in 0..data.len() {
        if test_index(&data, i, len) {
            return i + len;
        }
    }
    return 0;
}

fn main() {
    let data = fs::read("/home/kjaget/aoc2022/day6/input.txt").expect("Couldn't read file");

    println!("First start = {}", find_first_index(&data, 4));
    println!("First message = {}", find_first_index(&data, 14));
}