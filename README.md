Locks
-----

Implementation of some standard synchronization primitives.

# Contents
File | Description
---- | -----------
`locks/bakery.h` | [Lamport's Bakery algorithm](http://lamport.azurewebsites.net/pubs/bakery.pdf)
`locks/lamport.h` | [Lamport's Fast algorithm](http://delivery.acm.org/10.1145/10000/7352/p1-lamport.pdf?ip=128.151.150.18&id=7352&acc=ACTIVE%20SERVICE&key=7777116298C9657D%2EDC6AD36C640314EC%2E4D4702B0C3E38B35%2E4D4702B0C3E38B35&__acm__=1522462846_4dcb8e87413a27ff45cdedf439f0d7e1)
`locks/spin.h` | [test-and-test-and-set lock](http://delivery.acm.org/10.1145/810000/808203/p340-rudolph.pdf?ip=128.151.150.2&id=808203&acc=ACTIVE%20SERVICE&key=7777116298C9657D%2EDC6AD36C640314EC%2E4D4702B0C3E38B35%2E4D4702B0C3E38B35&__acm__=1522504209_8b4ece9e8a9e71aa1ae8df4980ea6f9f)
`locks/ticket.h` | Ticket lock
