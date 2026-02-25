# Rust 所有权(ownership)
`所有权(ownership)`让 Rust 无需垃圾回收（garbage collector）即可保障内存安全

## 内存与分配
内存分配器（memory allocator）在堆的某处找到一块足够大的空位，把它标记为已使用，并返回一个表示该位置地址的 指针（pointer）。这个过程称作 在堆上分配内存（allocating on the heap），有时简称为 “分配”（allocating）。（将数据推入栈中并不被认为是分配）。因为指向放入堆中数据的指针是已知的并且大小是固定的，你可以将该指针存储在栈上，不过当需要实际数据时，必须访问指针。

|规则|说明|参考文档|备注|
|-|-|-|-|
|- 已知固定大小的简单值，栈上分配|-|-|- let x = 5; let y = x;|
|-|-|-|-|
|-|-|-|-|
|-|-|-|-|
|-|-|-|-|
|-|-|-|-|

---


## 什么是所有权系统
跟踪哪部分代码正在使用堆上的哪些数据，最大限度的减少堆上的重复数据的数量，以及清理堆上不再使用的数据确保不会耗尽空间，这些问题正是所有权系统要处理的 [什么是所有权？:栈（Stack）与堆（Heap）](https://kaisery.github.io/trpl-zh-cn/ch04-01-what-is-ownership.html)



Keeping track of what parts of code are using what data on the heap, minimizing the amount of duplicate data on the heap, and cleaning up unused data on the heap so that you don’t run out of space are all problems that ownership addresses.<sup>跟踪堆中哪些代码部分使用了哪些数据，减少堆上的重复数据，以及清理堆中未使用的数据以防止空间不足，这些都是所有权会解决的问题</sup> -- From: [What Is Ownership? :The Stack and the Heap](https://doc.rust-lang.org/book/ch04-01-what-is-ownership.html)

Rust takes a different path: The memory is automatically returned once the variable that owns it goes out of scope. （Rust 采取了一个不同的策略：内存在拥有它的变量离开作用域后就被自动释放）


### 所有权规则
|规则|说明|备注|
|-|-|-|
|- Rust 中的每一个值都有一个 所有者（owner）|-|- 所有者，堆上数据指针?错！所有者 = 资源管理器 + 生命周期控制器|
|-|-|-|
|- 值在任一时刻有且只有一个所有者|-|-|
|-|-|-|
|- 当所有者离开作用域，这个值将被丢弃|-|-|
|-|-|-|

---

### “drop” 函数
当变量离开作用域，Rust 为我们调用一个特殊的函数。这个函数叫做 drop，在这里 String 的作者可以放置释放内存的代码。Rust 在结尾的 } 处自动调用 drop。 <sup>对于 String 类型，为了支持一个可变，可增长的文本片段，需要在堆上分配一块在编译时未知大小的内存来存放内容</sup>
```txt
 实现逻辑: 调试 rust 源码 ...
```

只有拥有所有权的变量，在超出作用域范围的时候，才会调用“drop”函数释放空间;

---
 

### 移动(move)
```rust
// 对于编译时已知大小的类型被整个存储在栈上
// ===> 只在栈上的数据：拷贝

    let x = 5;
    let y = x;

    println!("x = {x}, y = {y}");

// x 依然有效且没有被移动到 y 中

// 对于 String 类型，为了支持一个可变，可增长的文本片段，需要在堆上分配一块在编译时未知大小的内存来存放内容
    let mut s = String::from("hello");
    s = String::from("ahoy");

    println!("{s}, world!");
   
   // s1 被 移动 到了 s2 , s1失效了，s2 是有效的，当其离开作用域，它就释放自己的内存
```

### 作用域与复制
当你给一个已有的变量赋一个全新的值时，Rust 将会立即调用 drop 并释放原始值的内存

```rust
    let mut s = String::from("hello");
    s = String::from("ahoy"); // 原始的字符串(hello)立刻就离开了作用域

    println!("{s}, world!");


```

### 所有权与函数
> 将值传递给函数与给变量赋值的原理相似

---

### 引用与借用
A reference is like a pointer in that it’s an address we can follow to access the data stored at that address; that data is owned by some other variable. Unlike a pointer, a reference is guaranteed to point to a valid value of a particular type for the life of that reference.<sup>引用（reference）像一个指针，因为它是一个地址，我们可以由此访问储存于该地址的属于其他变量的数据。与指针不同，引用在其生命周期内保证指向某个特定类型的有效值。</sup>

 These ampersands represent references, and they allow you to refer to some value without taking ownership of it.<sup>这些 & 符号就是 引用，它们允许你使用值但不获取其所有权</sup>

 We call the action of creating a reference borrowing. <sup>我们将创建一个引用的行为称为 借用（borrowing）</sup>

  ```rust
  
  fn main() {
      let s1 = String::from("hello");
  
      let len = calculate_length(&s1);
  
      println!("The length of '{s1}' is {len}.");
  }
  
  fn calculate_length(s: &String) -> usize {
      s.len()
  }
  
```

#### 引用是不可变的
Just as variables are immutable by default, so are references. We’re not allowed to modify something we have a reference to.<sup>正如变量默认是不可变的，引用也一样。（默认）不允许修改引用的值。： 无法修改引用变量的值</sup> 
```rust
fn main() {
    let s = String::from("hello");

    change(&s);
}

//some_string.push_str(", world");
//  |     ^^^^^^^^^^^ `some_string` is a `&` reference, so it cannot be borrowed as mutable 
fn change(some_string: &String) {
    some_string.push_str(", world");
}
```

#### 可变引用（Mutable References）
允许修改借用的值——可变引用（mutable reference）
```rust
   fn main() {
       let mut s = String::from("hello");
   
       change(&mut s);
   }
   
   fn change(some_string: &mut String) {
       some_string.push_str(", world");
   }
```


---

### 悬垂指针（Dangling References）
n languages with pointers, it’s easy to erroneously create a dangling pointer—a pointer that references a location in memory that may have been given to someone else—by freeing some memory while preserving a pointer to that memory. <sup>在具有指针的语言中，很容易通过释放内存时保留指向它的指针而错误地生成一个悬垂指针（dangling pointer）—— 指向可能已被分配给其他用途的内存位置的指针。</sup>
> 指向的内存已经不是使用的那个，原来的那个已经被释放了

---

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
|- cargo asm ownership::main|- 查看汇编|- ownership git:(master) ✗ cargo install cargo-show-asm|- ➜  ownership git:(master) ✗ cargo asm ownership::main|- 内存分配: bl __rustc::__rust_alloc <br/> - 内存释放: bl __rustc::__rust_dealloc|
|-|-|-|-|-|
|- cargo expand|- 查看宏展开|- ➜  ownership git:(master) ✗ cargo install cargo-expand |- ➜  ownership git:(master) ✗ cargo expand |-|
|-|-|-|-|-|
|-|-|-|-|-|
|-|-|-|-|-|

