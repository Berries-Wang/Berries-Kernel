# 闭包

## 什么是闭包?
闭<sup>是指它把外部的变量“关”进了自己的作用域</sup>包<sup>是指它像一个包一样，包含了执行逻辑和所需的数据</sup> = 函数 + 引用环境

这个词源于数学。它指的是一个函数连同它引用的外部变量，形成了一个封闭的包裹:(闭包就是代码和数据的结合体。它让函数拥有了“记忆”，能够记住它出生时的样子)

|闭包的三个核心特征|说明|备注|
|-|-|-|
|- 匿名性|- 闭包通常没有名字。在 Rust 中用   &#124; err &#124; &#123; ... &#125;  |- Rust 用 \| \| \{ \}|
|-|-|-|
|- 捕获环境|- 这是闭包区别于普通函数的地方。普通函数只能使用参数和全局变量，而闭包可以“捕获”它被定义时所处环境中的局部变量|-|
|-|-|-|
|- 延迟执行|- 闭包定义好后，并不会立即运行，而是可以像变量一样被传递，在未来的某个时刻、某个位置被触发|-|


Rust 的 闭包（closures）是可以保存在变量中或作为参数传递给其他函数的匿名函数, 可以在一个地方创建闭包，然后在不同的上下文中执行闭包运算。不同于函数，闭包允许捕获其被定义时所在作用域中的值

---

## 什么是闭包?
> 阅读: [MDN:闭包](https://developer.mozilla.org/zh-CN/docs/Web/JavaScript/Guide/Closures) : 或 [MDN:闭包: PDF](./999.REFS/闭包%20-%20JavaScript%20_%20MDN.pdf) <sup>闭包是由捆绑起来（封闭的）的函数和函数周围状态（词法环境）的引用组合而成。换言之，闭包让函数能访问它的外部作用域。</sup> <br/> & [MDN:Closures](https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Closures#closure) 或 [MDN:Closures: PDF](./999.REFS/Closures%20-%20JavaScript%20_%20MDN.pdf) <br/> & [Rust语言圣经(Rust Course):闭包 Closure](https://course.rs/advance/functional-programing/closure.html) <sup>闭包是一种匿名函数，它可以赋值给变量也可以作为参数传递给其它函数，不同于函数的是，它允许捕获调用者作用域中的值</sup>: [Rust语言圣经(Rust Course): 闭包： PEF](./999.REFS/闭包%20Closure%20-%20Rust语言圣经(Rust%20Course).pdf) <br/>

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