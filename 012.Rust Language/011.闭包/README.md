# 闭包（Closures）： 可以捕获环境的匿名函数
## 什么是闭包?
闭<sup>是指它把外部的变量“关”进了自己的作用域</sup>包<sup>是指它像一个包一样，包含了执行逻辑和所需的数据</sup> = 函数 + 引用环境

这个词源于数学。它指的是一个函数连同它引用的外部变量，形成了一个封闭的包裹:(闭包就是代码和数据的结合体。它让函数拥有了“记忆”，能够记住它出生时的样子)

<div>
<table>
    <thead>
        <tr>
            <th>闭包的三个核心特征</th>
            <th>说明</th>
            <th>备注</th>
        </tr>
    </thead>
    <tbody>
        <tr>
            <td>匿名性</td>
            <td>闭包通常没有名字。在 Rust 中用 | | { } 表示</td>
            <td></td>
        </tr>
        <tr><td></td><td></td><td></td></tr>
        <tr>
            <td>捕获环境</td>
            <td>这是闭包区别于普通函数的地方。普通函数只能使用参数和全局变量，而闭包可以“捕获”它被定义时所处环境中的局部变量</td>
            <td></td>
        </tr>
        <tr><td></td><td></td><td></td></tr>
        <tr>
            <td>延迟执行</td>
            <td>闭包定义好后，并不会立即运行，而是可以像变量一样被传递，在未来的某个时刻、某个位置被触发</td>
            <td></td>
        </tr>
    </tbody>
</table>
</div>


### 权威参考资料
Rust 的 闭包（closures）是可以保存在变量中或作为参数传递给其他函数的匿名函数, 可以在一个地方创建闭包，然后在不同的上下文中执行闭包运算。不同于函数，闭包允许捕获其被定义时所在作用域中的值。<sup>from [《Rust 程序设计语言》 中文版](https://rust-lang.org/zh-CN/learn/)</sup>

Rust’s closures are anonymous functions you can save in a variable or pass as arguments to other functions. You can create the closure in one place and then call the closure elsewhere to evaluate it in a different context. Unlike functions, closures can capture values from the scope in which they’re defined. <sup>from [The Rust Programming Language · 原版](https://rust-lang.org/zh-CN/learn/): [The Rust Programming Language : Closures](https://doc.rust-lang.org/book/ch13-01-closures.html)</sup>
+ 译文: Rust 的闭包是一种匿名函数，你可以将其保存到变量中，或作为参数传递给其他函数。你可以在一处创建闭包，然后在另一处调用它，从而在不同的上下文中对其求值。与函数不同，闭包可以捕获其定义时所在作用域中的值。我们将演示闭包的这些特性如何实现代码复用及行为自定义。

---

## 什么是闭包?
> 阅读: [MDN:闭包](https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Closures) : 或 [MDN:闭包: PDF](./999.REFS/闭包%20-%20JavaScript%20_%20MDN.pdf) <sup>闭包是由捆绑起来（封闭的）的函数和函数周围状态（词法环境）的引用组合而成。换言之，闭包让函数能访问它的外部作用域。</sup> <br/> & [MDN:Closures](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Closures#closure) 或 [MDN:Closures: PDF](./999.REFS/Closures%20-%20JavaScript%20_%20MDN.pdf) <br/> 


---

## 使用闭包捕获环境
```rust
#[derive(Debug, PartialEq, Copy, Clone)]
enum ShirtColor {
    Red,
    Blue,
}

struct Inventory {
    shirts: Vec<ShirtColor>,
}

impl Inventory {
    fn giveaway(&self, user_preference: Option<ShirtColor>) -> ShirtColor {
        // Option<T> 上的 unwrap_or_else 方法由标准库定义。它接受一个无参闭包作为参数，该闭包返回一个 T 类型的值
        // --> 闭包捕获了对 self（即 Inventory 实例）的不可变引用，并将其与我们指定的代码一起传递给 unwrap_or_else 方法
        // --> 在这里定义了闭包，而 unwrap_or_else 的实现会在之后需要其结果的时候执行闭包
        user_preference.unwrap_or_else(|| self.most_stocked())
    }

    fn most_stocked(&self) -> ShirtColor {
        let mut num_red = 0;
        let mut num_blue = 0;

        for color in &self.shirts {
            match color {
                ShirtColor::Red => num_red += 1,
                ShirtColor::Blue => num_blue += 1,
            }
        }
        if num_red > num_blue {
            ShirtColor::Red
        } else {
            ShirtColor::Blue
        }
    }
}

fn main() {
    let store = Inventory {
        shirts: vec![ShirtColor::Blue, ShirtColor::Red, ShirtColor::Blue],
    };

    let user_pref1 = Some(ShirtColor::Red);
    let giveaway1 = store.giveaway(user_pref1);
    println!(
        "The user with preference {:?} gets {:?}",
        user_pref1, giveaway1
    );

    let user_pref2 = None;
    let giveaway2 = store.giveaway(user_pref2);
    println!(
        "The user with preference {:?} gets {:?}",
        user_pref2, giveaway2
    );
}
```


---

## 闭包类型推断和注解
- 闭包通常不要求像 fn 函数那样对参数和返回值进行类型注解。函数需要类型注解是因为这些类型是暴露给用户的显式接口的一部分。严格定义这些接口对于确保所有人对函数使用和返回值的类型达成一致理解非常重要。
- 闭包并不用于这样暴露在外的接口：它们储存在变量中并被使用，不用命名它们或暴露给库的用户调用
- 在某些罕见的情况下，编译器也需要闭包的类型注解
  ```rust
   let expensive_closure = |num: u32| -> u32 {
        println!("calculating slowly...");
        std::thread::sleep(std::time::Duration::from_secs(2));
        num
    };
  ```
- 对于闭包定义，编译器会为每个参数和返回值推断出一个具体类型： 这些类型被锁定进闭包 example_closure 中，如果尝试对同一闭包使用不同类型则就会得到类型错误
  ```rust
    let example_closure = |x| x;

    let s = example_closure(String::from("hello")); // 这会推断出 example_closure 的参数和返回值类型为 String
    let n = example_closure(5); // 这会导致编译错误，因为 example_closure 已经被推断为接受 String 类型的参数，而不是整数 ： 错误信息:  ^ expected `String`, found integer
  ```

---

## 捕获引用或移动所有权
闭包可以通过三种方式捕获其环境中的值，它们直接对应到函数获取参数的三种方式：
+ `不可变借用` : 捕获/借用，不会导致所有权转移
  ```rust
    fn main() {
       let list = vec![1, 2, 3];
       println!("Before defining closure: {list:?}");
   
       // 捕获名为 list 的 vector 的不可变引用的闭包
       // 因为同时可以有多个 list 的不可变引用，所以在闭包定义之前，闭包定义之后调用之前，闭包调用之后代码仍然可以访问 list
       let only_borrows = || println!("From closure: {list:?}");
   
       println!("Before calling closure: {list:?}");
       only_borrows();
       println!("After calling closure: {list:?}");
    }
  ```
+ `可变借用`: 捕获/借用，不会导致所有权转移
    ```rust
    fn main() {
       let mut list = vec![1, 2, 3];
       println!("Before defining closure: {list:?}");
   
       let mut borrows_mutably = || list.push(7);

       // 因为当可变借用存在时不允许有其它的借用，所以在闭包定义和调用之间不能有不可变引用来进行打印。
       //这行添加之后会报错:  println!("After calling closure: {list:?}"); // 错误信息: error[E0502]: cannot borrow `list` as immutable because it is also borrowed as mutable
       // {list:?}: immutable borrow occurs here
   
       borrows_mutably();
       // 闭包在被调用后就不再被使用，这时可变借用结束。
       println!("After calling closure: {list:?}");
   }
    ```
+ `获取所有权`： 即使闭包体不严格需要所有权，如果希望强制闭包获取它在环境中所使用的值的所有权，可以在参数列表前使用 move 关键字
    ```rust
        // 当将闭包传递到一个新的线程时，这个技巧特别有用，因为它将数据的所有权移动到新线程中
        use std::thread;

     fn main() {
         let list = vec![1, 2, 3];
         println!("Before defining closure: {list:?}");
     
         // 闭包仅通过不可变引用捕获了 list，因为这是打印列表所需的最小访问权限
         std::thread::spawn(move || println!("From thread: {list:?}"))
             .join()
             .unwrap();
     }
    ```

---

## 将捕获的值移出闭包和 Fn trait ？
一旦闭包捕获了定义它的环境中的某个值的引用或所有权（也就影响了什么会被移进闭包，如有），闭包体中的代码则决定了在稍后执行闭包时，这些引用或值将如何处理，闭包体可以执行以下任一操作：
- 将一个捕获的值移出闭包，修改捕获的值
- 既不移动也不修改值
- 或者一开始就不从环境中捕获任何值


按 闭包捕获和处理环境中的值的方式 ，闭包可以实现以下三个 trait 中的一个或多个：
|Fn trait 类型|说明|备注|
|-|-|-|
|- FnOnce|- FnOnce适用于只能被调用一次的闭包。 所有闭包至少都实现了这个 trait，因为所有闭包都能被调用。一个会将捕获的值从闭包体中移出的闭包只会实现 FnOnce trait，而不会实现其他 Fn 相关的 trait，因为它只能被调用一次。|- 会将捕获的值移出闭包|
|-|-|-|
|- FnMut|- FnMut 适用于不会将捕获的值移出闭包体<sup>移出闭包体，是针对于捕获的值，而不是说这个闭包不返回值!!!</sup>，但可能会修改捕获值的闭包。|- 修改捕获的值|
|-|-|-|
|- Fn|- Fn 适用于既不将捕获的值移出闭包体，也不修改捕获值的闭包，同时也包括不从环境中捕获任何值的闭包。|- 这类闭包可以被多次调用而不会改变其环境，这在会多次并发调用闭包的场景中十分重要 <br/> - **既不会将捕获的值移出闭包，也不会修改闭包的值;**|

### 为什么要设计这么多类型呢？
Rust 区分这三种 Trait 是为了在内存安全的前提下，尽可能提高代码的性能和复用性。



### 代码示例
#### FnOnce
```rust
impl<T> Option<T> {
    // 泛型 F 的 trait bound 是 FnOnce() -> T，这意味着 F 必须能够被调用一次，没有参数并返回一个 T。
    pub fn unwrap_or_else<F>(self, f: F) -> T
    where
        F: FnOnce() -> T
    {
        match self {
            Some(x) => x,
            None => f(),
        }
    }
}
```