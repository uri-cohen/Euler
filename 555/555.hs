module Main (main) where

import Math.NumberTheory.Primes.Factorisation (divisors)
import Data.Set as Set

-- def S(p,m):
--     sum = 0
--     for k in range(1,p+1):
--         for ds in sympy.divisors(k)[:-1]:
--             s = k - ds
--             first = m - s + 1
--             last = min(m, m - s + ds)
--             sum += (last-first+1)*(first+last)/2
--     return sum

s :: Int -> Int -> Int
s p m = sum $ Prelude.map (smk m) [1 .. p]

smk :: Int -> Int -> Int
smk m k =
    sum $ Prelude.map (smkd m k) ds
    where ds = [fromInteger d | d <- Set.elems $ divisors (toInteger k), (fromInteger d) /= k]

smkd :: Int -> Int -> Int -> Int
smkd m k d =
    d * (first + last) `quot` 2
    where s = k - d
          first = m - s + 1
          last = m - s + d

main :: IO ()
main = do
    putStr "hi there "
    putStrLn $ show $ s 1000000 1000000

