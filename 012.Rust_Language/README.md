# Rust 学习
Rust 是 静态类型（statically typed）语言，也就是说在编译时就必须知道所有变量的类型。

## 一些‘名词’
|名词|说明|备注|
|-|-|-|
|- 关联函数（associated function）|- 关联函数是针对某个类型实现的函数|- 例如: let mut guess = String::new(); new 就是String的关联函数|
|-|-|-|
|- & (引用（reference）)|- 允许多处代码访问同一处数据，而无需在内存中多次拷贝|- std::io::stdin().read_line(&mut guess).expect("Faild to read line");|
|-|-|-|
|- range expression(范围表达式)|-|- 形式: <br/> + start..=end :     let secret_number = rand::thread_rng().gen_range(1..=100);|
|-|-|-|
|- match 表达式|- 由 分支（arms） 构成。一个分支包含一个 模式（pattern）和表达式开头的值与分支模式相匹配时应该执行的代码|-|
|-|-|-|
|- 遮蔽（Shadowing）|-|-|
|-|-|-|
|- 解构（destructuring|-|-|
|-|-|-|
|- 语句|- 语句（Statements）是执行一些操作但不返回值的指令|-|
|-|-|-|
|- 表达式|- 表达式（Expressions）计算并产生一个值|- Rust是基于表达式（expression-based）的语言|
|-|-|-|
|- 函数的返回值|- 在 Rust 中，函数的返回值等同于函数体最后一个表达式的值。|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|
|-|-|-|

---

## 示例
### 猜数游戏
```rust
/**
 * Cargo.toml
 *
 * [package]
 * name = "guessing_game"
 * version = "0.1.0"
 * edition = "2024"
 * 
 * [dependencies]
 * rand = "0.8.5"
*/
use rand::Rng;
use std::cmp::Ordering;

fn main() {
    println!("Guess the number!");

    let secret_number = rand::thread_rng().gen_range(1..=100);
    println!("The secret number is: {secret_number}");

    loop {
        let mut guess: String = String::new();

        println!("Please input your guess.");

        std::io::stdin()
            .read_line(&mut guess)
            .expect("Faild to read line");

        /*
         * 居然可以直接修改类型
         * Rust 允许用一个新值来 遮蔽 （Shadowing） guess 之前的值
         *
         * parse 方法 将字符串转换成其他类型  , 转成什么类型，由变量类型指定(let guess: u32 中的u32)
         */
        // let guess: u32 = guess.trim().parse().expect("Please type a number!");

        /*
         * _ 是一个通配符值
         */
        let guess: u32 = match guess.trim().parse() {
            Ok(num) => num,
            Err(_) => continue,
        };
        println!("Your guessed: {guess}");

        match guess.cmp(&secret_number) {
            Ordering::Less => println!("Too small!"),
            Ordering::Equal => {
                println!("You win!");
                break;
            }
            Ordering::Greater => println!("Too big!"),
        }
    }
}
```