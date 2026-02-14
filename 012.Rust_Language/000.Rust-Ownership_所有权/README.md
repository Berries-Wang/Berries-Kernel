# Rust 所有权(ownership)
`所有权(ownership)`让 Rust 无需垃圾回收（garbage collector）即可保障内存安全


## 分析工具
```rust
// 项目名: ownership
fn main() {
    println!("Hello, world!");
    {
        let s = String::from("hello");
        println!("{s}");
    }
}

```

|工具名称|功能|安装|命令|备注|
|-|-|-|-|-|
|- cargo asm ownership::main|- 查看汇编|- ownership git:(master) ✗ cargo install cargo-show-asm|- ➜  ownership git:(master) ✗ cargo asm ownership::main|-|
|-|-|-|-|-|
|- cargo expand|- 查看宏展开|- ➜  ownership git:(master) ✗ cargo install cargo-expand |- ➜  ownership git:(master) ✗ cargo expand |-|
|-|-|-|-|-|
|-|-|-|-|-|
|-|-|-|-|-|

