fn (main argc argv) {
    (print (fib 5000))
    (putStrLn (show (fac 5)))
    df ((f5) {(float (fib 5))}
        (answer) {5.0}) {
        (putStrLn
            (concat [
                (show f5) " =? " (show answer)
                "->" (show (= answer f5))
            ]))
    }
    0
}

fn (avg2 x y) {
    (/ (+ x y) 2)
}

fn (fib n) {
    df ((x) {(- n 1)}
        (y) {(- n 2)}) {
        (+ (fib x) (fib y))
    }
}

fn (fac n) {
    (* n (fac (- n 1)))
}
