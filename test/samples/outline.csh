fn (main argc argv) {
    (print (fib 5))
    (putStrLn (show (fac 5)))
    0
}

fn (fib n) {
    vr ((= x (- n 1))
        (= y (- n 2))) {
        (+ (fib x) (fib y))
    }
}

fn (fac n) {
    (* n (fac (- n 1)))
}
