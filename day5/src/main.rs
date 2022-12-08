use std::io;
use std::io::prelude::*;
use std::fs::File;
use std::io::BufReader;
use std::vec::Vec;
use std::collections::VecDeque;

use itertools::Itertools;
use regex::Regex;

struct Move {
    count: usize,
    from_idx: usize,
    to_idx: usize,
}

fn read_move(line: String) -> Option<Move>
{
    let re = Regex::new(r"move (\d+) from (\d+) to (\d+)").unwrap();
    match re.captures(&line) {
                Some(cap) => {
                    if cap.len() > 3 {
                        return  Some(Move {
                            count: cap[1].parse::<usize>().unwrap(),
                            from_idx: cap[2].parse::<usize>().unwrap(),
                            to_idx: cap[3].parse::<usize>().unwrap(),
                        } );
                    }
                    return None;
                }
                None => { return None; }
            }
}

fn process_move_9000(this_move: &Move, stacks: &mut Vec<VecDeque<char>>) {
    for _ in 0..this_move.count {
        match stacks[this_move.from_idx - 1].pop_back() {
            Some(val)  => { stacks[this_move.to_idx - 1].push_back(val); }
            None => {}
        }
    }
}

fn process_move_9001(this_move: &Move, stacks: &mut Vec<VecDeque<char>>) {
    let mut queue = VecDeque::new();
    for _ in 0..this_move.count {
        match stacks[this_move.from_idx - 1].pop_back() {
            Some(val) => { queue.push_back(val); }
            None => {}
        }
    }
    for _ in 0..this_move.count {
        match queue.pop_back() {
            Some(val) => { stacks[this_move.to_idx - 1].push_back(val); }
            None => {}
        }
    }
}

fn read_stacks<R: BufRead>(f: &mut R)-> Vec<VecDeque<char>>
{
    let mut ret = Vec::new();
    let re = Regex::new(r"\[([A-Z])\]").unwrap();
    for line in f.lines() {
        if line.as_ref().unwrap().len() < 2 {
            break;
        }
        let mut stack_strings = Vec::new();
    
        for chunk in &line.as_ref().unwrap().chars().chunks(4) {
            stack_strings.push(String::from_iter(chunk));
        }
        while ret.len() < stack_strings.len() {
            ret.push(VecDeque::new());
        }
        //println!("Stack strings =  {:?}", stack_strings);
        for (i, s) in stack_strings.iter().enumerate() {
            match re.captures(&s) {
                Some(cap) => {
                    if cap.len() > 1 {
                        ret[i].push_front(cap[1].chars().nth(0).unwrap());
                    }
                }
                None => { }
            }
        }
    }

    return ret;
}

fn main() -> io::Result<()> {
    let file_name = File::open("/home/kjaget/aoc2022/day5/input.txt")?;
    let mut reader = BufReader::new(file_name);

    let mut stacks_9000 = read_stacks(& mut reader);
    let mut stacks_9001 = stacks_9000.clone();

    println!("Stacks = {:?}", stacks_9000);

    for line in reader.lines() {
        match read_move(line.unwrap()) {
            Some(next_move) => {
                process_move_9000(&next_move, &mut stacks_9000);
                process_move_9001(&next_move, &mut stacks_9001);
            }
            None => { }
        }
    }
    for s in stacks_9000 {
        print!("{}", s.back().unwrap());
    }
    println!("");
    for s in stacks_9001 {
        print!("{}", s.back().unwrap());
    }
    println!("");
    Ok(())
}