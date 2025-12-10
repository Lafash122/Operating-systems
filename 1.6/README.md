Разработка собственной функции для создания ядерных потоков - аналог pthread_create():
`int mythread_create(mythread_t thread, void *(*start_routine)(void *), void *arg);`
Функция возвращает успех-неуспех и оформлена в виде библиотеки
