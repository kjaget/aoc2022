use std::io;
use std::io::prelude::*;
use std::fs::File;
use std::io::BufReader;

use std::collections::HashSet;
use regex::Regex;

fn process_line(l : String) -> (bool, bool) {
    let re = Regex::new(r"(\d+)-(\d+),(\d+)-(\d+)").unwrap();
    let cap = re.captures(&l).unwrap();

    let sections1 = HashSet::from_iter(cap[1].parse::<u64>().unwrap(), cap[2].parse::<u64>().unwrap());
    let sections2 = HashSet::from_iter(cap[3].parse::<u64>().unwrap(), cap[4].parse::<u64>().unwrap());

    let intersection = sections1.intersection(&sections2).copied().collect();
    let full_overlap = (sections1 == intersection) || (sections2 == intersection);
    let any_overlap = intersection.len() > 0;
    (full_overlap, any_overlap)
}

fn main() -> io::Result<()> {
    let file_name = File::open("/home/kjaget/aoc2022/day4/input.txt")?;
    let reader = BufReader::new(file_name);
    let lines = reader.lines();
    
    let mut full_count: u64 = 0;
    let mut any_count: u64 = 0;
    for line in lines {
        let (full, any) = process_line(line.unwrap());
        if full {
            full_count += 1;
        }
        if any {
            any_count += 1;
        }

    }
    println!("Full count = {}", full_count);
    println!("Any  count = {}", any_count);

    Ok(())
}