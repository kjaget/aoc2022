use std::io;
use std::io::prelude::*;
use std::fs::File;
use std::io::BufReader;

use substring::Substring;
use std::collections::HashMap;

struct MyFile{
    name: String,
    size: usize,

}

struct MyDir {
    name: String,
    files: Vec<MyFile>,
    subdirs: HashMap<String, MyDir>,
    size: usize,
}

impl MyDir {
    fn new(dirname: String) -> Self {
        Self{name: dirname, files: vec![], subdirs: HashMap::new(), size: 0}
    }
    fn creat(&mut self, path_list: Vec<String>, filename: String, filesize: usize) {
        let mut d = self;
        for p in path_list {
            d = d.subdirs.get_mut(&p).unwrap();
        }
        d.files.push(MyFile{name: filename.to_string(), size: filesize});
    }
    /*
    fn add_file(& mut self, dirname: String, filename: String, filesize: usize) -> bool {
        if dirname == self.name {
            self.files.push(MyFile{name: filename.to_string(), size: filesize});
            return true;
        }
        for s in &mut self.subdirs {
            if s.add_file(dirname.clone(), filename.clone(), filesize) {
                return true;
            };
        }
        return false;
    }
    */
    fn chdir(&mut self, dirname: String) -> Option<&mut MyDir> {
        return self.subdirs.get_mut(&dirname);
    }
    fn mkdir(& mut self, dirname: String) {
        self.subdirs.insert(dirname.clone(), MyDir::new(dirname));
    }
}

fn main() -> io::Result<()> {
    let file_name = File::open("/home/kjaget/aoc2022/day7/input.txt")?;
    let reader = BufReader::new(file_name);
    let lines = reader.lines();
    
    let mut root_dir = MyDir::new("/".to_string());
    let mut path_stack: Vec<Option<&mut MyDir>> = vec![];
    for line in lines {

        match "true" {
            line if line.starts_with("$ ls") => { 
            }
            line if line.starts_with("$ cd ") => { 
                match line.substring(5, line.len()) {
                    ".." => { path_stack.pop(); }
                    "/"  => { path_stack.clear(); }
                    dirname =>  {let mut cwd = &mut path_stack.last().unwrap().as_ref().unwrap(); 
                        path_stack.push(cwd.chdir(dirname.to_string()));}

                }
            }
            line if line.starts_with("dir ") => { }
            _ =>  {}
        }
    }

    Ok(())
}
