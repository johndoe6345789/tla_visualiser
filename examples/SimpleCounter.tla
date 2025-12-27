---------------------------- MODULE SimpleCounter ----------------------------
(* A simple counter specification for testing TLA+ Visualiser *)

EXTENDS Naturals

VARIABLE counter

Init == counter = 0

Increment == counter' = counter + 1

Decrement == counter' = counter - 1

Next == Increment \/ Decrement

Spec == Init /\ [][Next]_counter

TypeInvariant == counter \in Nat

=============================================================================
