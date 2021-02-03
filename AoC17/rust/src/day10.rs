use std::ops::{Index, IndexMut, Range};
use std::str::FromStr;

struct Circle<T> {
    inner: Vec<T>,
}

impl<T> Circle<T> {
    fn new(inner: Vec<T>) -> Self {
        Circle { inner }
    }
}

impl<T> Index<usize> for Circle<T> {
    type Output = T;

    fn index(&self, index: usize) -> &T {
        &self.inner[index % self.inner.len()]
    }
}

impl<T> IndexMut<usize> for Circle<T>
where Circle<T>: Index<usize, Output=T> {
    fn index_mut(&mut self, index: usize) -> &mut T {
        let l = self.inner.len();
        &mut self.inner[index % l]
    }
}

fn swap<T, I>(i: &mut I, ai: usize, bi: usize)
where I: IndexMut<usize, Output=T>, T: Copy {
    let av = i[ai];
    i[ai] = i[bi];
    i[bi] = av;
}

fn reverse<T, I>(i: &mut I, range: Range<usize>)
where I: IndexMut<usize, Output=T>, T: Copy {
    let half = range.len() / 2;
    for di in 0..half {
        swap(i, range.start + di, range.end - di -1);
    }
}

pub fn task1(n: usize, l: &str) -> usize {
    // https://doc.rust-lang.org/std/ops/struct.Range.html
    let list: Vec<usize> = (0..n).collect();
    let mut circle = Circle::new(list);
    let lengths: Vec<usize> = l
        .split(",")
        .map(|e| usize::from_str(e.trim()).unwrap())
        .collect();
    let mut cur_pos: usize = 0;
    let mut skip_size: usize = 0;
    for cur_len in lengths {
        reverse(&mut circle, cur_pos..(cur_pos + cur_len));
        cur_pos = (cur_pos + cur_len + skip_size);
        skip_size += 1;
    }
    circle[0] * circle[1]
}

fn to_hex(n: usize) -> String {
    format!("{:01$x}", n, 2)
}

fn xor<T, I>(i: &I, start: usize) -> T
    where I: IndexMut<usize, Output=T>, T: Copy, T: std::ops::BitXorAssign {
    let n = 16;
    let mut result = i[start];
    for di in 1..n {
        result ^= i[start + di];
    }
    result
}

pub fn task2(n: usize, l: &str) -> String {
    let list: Vec<usize> = (0..n).collect();
    let mut circle = Circle::new(list);
    let nl = l.trim();
    let mut tail = vec![17, 31, 73, 47, 23];
    let mut head: Vec<usize> = nl.chars().map(|x| x as usize).collect();
    head.append(&mut tail);
    let lengths: Vec<usize> = head;
    let mut cur_pos = 0;
    let mut skip_size = 0;
    for _ in 0..64 {
        for cur_len in &lengths {
            reverse(&mut circle, cur_pos..(cur_pos + *cur_len));
            cur_pos = (cur_pos + cur_len + skip_size);
            skip_size += 1;
        }
    }
    let mut result = String::new();
    for i in 0..16 {
        result.push_str(&to_hex(xor(&circle, i * 16)));
    }
    assert_eq!(32, result.len());
    result
}
