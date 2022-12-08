use std::io;
use std::io::prelude::*;
use std::fs::File;
use std::io::BufReader;

fn read_next_elf<R: BufRead>(f: &mut R) -> u64
{
    let mut calories = 0u64;

    loop {
        let mut line: String = "".to_string();
        match f.read_line(& mut line)
        {
            bytes @ Ok(_) => {
                if bytes.unwrap() < 2 {
                    return calories;
                }
                calories += line.replace('\n',"").parse::<u64>().unwrap();
            }
            _ => { 
                println!("Invalid line read");
                return 0;
            }
        }
    }
}

fn main() -> io::Result<()> {
    let file_name = File::open("./input.txt")?;
    let mut f = BufReader::new(file_name);

    let mut max_calories : [u64; 4] = [0u64; 4];
    loop {
        max_calories[0] = read_next_elf(& mut f);

        if max_calories[0] == 0u64 {
            break;
        }
        max_calories.sort();
    }
    println!("Max calories = {}", max_calories[3]);
    println!("Max 3 calories = {}", max_calories[1] + max_calories[2] + max_calories[3]);

    Ok(())
}