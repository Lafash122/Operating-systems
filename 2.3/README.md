Реализация односвязного списка, хранящего строки длиной менее 100 символов, у которого с каждым элементом связан отдельный примитив синхронизации: спинлок, мьютекс, блокировка чтения-записи
## Замеры
### Мьютекс
- размер 100:
```bash
Mutex size: 100 -- on: 10 seconds
iters_incr: 467438, cnt_incr: 22723177, swap_incr: 277697
iters_decr: 465018, cnt_decr: 22725464, swap_decr: 278531
iters_comp: 477184, cnt_comp: 658988, swap_comp: 560535
```
- размер 1000:
```bash
Mutex size: 1000 -- on: 10 seconds
iters_incr: 108801, cnt_incr: 53850139, swap_incr: 145602
iters_decr: 108648, cnt_decr: 53771707, swap_decr: 144541
iters_comp: 108970, cnt_comp: 995878, swap_comp: 289402
```
- размер 10000:
```bash
Mutex size: 10000 -- on: 10 seconds
iters_incr: 12363, cnt_incr: 61076713, swap_incr: 17808
iters_decr: 12363, cnt_decr: 61224964, swap_decr: 17556
iters_comp: 12362, cnt_comp: 1315908, swap_comp: 35722
```
- размер 100000:
```bash
Mutex size: 100000 -- on: 10 seconds
iters_incr: 1294, cnt_incr: 64049283, swap_incr: 1799
iters_decr: 1294, cnt_decr: 64030734, swap_decr: 1870
iters_comp: 1295, cnt_comp: 1319743, swap_comp: 3739
```

### Спинлок
- размер 100:
```bash
Spinlock size: 100 -- on: 10 seconds
iters_incr: 1938486, cnt_incr: 94640750, swap_incr: 2209274
iters_decr: 1935072, cnt_decr: 94724433, swap_decr: 2216601
iters_comp: 1967620, cnt_comp: 2363863, swap_comp: 4351722
```
- размер 1000:
```bash
Spinlock size: 1000 -- on: 10 seconds
iters_incr: 233958, cnt_incr: 116018338, swap_incr: 379653
iters_decr: 232641, cnt_decr: 114803756, swap_decr: 368492
iters_comp: 237393, cnt_comp: 2285809, swap_comp: 732681
```
- размер 10000:
```bash
Spinlock size: 10000 -- on: 10 seconds
iters_incr: 20391, cnt_incr: 101391739, swap_incr: 39331
iters_decr: 20396, cnt_decr: 100421083, swap_decr: 36499
iters_comp: 20428, cnt_comp: 2105441, swap_comp: 73565
```
- размер 100000:
```bash
Spinlock size: 100000 -- on: 10 seconds
iters_incr: 2583, cnt_incr: 127296069, swap_incr: 3650
iters_decr: 2587, cnt_decr: 128610696, swap_decr: 3976
iters_comp: 2643, cnt_comp: 2649871, swap_comp: 7159
```

### RWlock
- размер 100:
```bash
Rwlock size: 100 -- on: 10 seconds
iters_incr: 538860, cnt_incr: 26361811, swap_incr: 279990
iters_decr: 538771, cnt_decr: 26422574, swap_decr: 279357
iters_comp: 563687, cnt_comp: 582435, swap_comp: 556915
```
- размер 1000:
```bash
Rwlock size: 1000 -- on: 10 seconds
iters_incr: 105682, cnt_incr: 52142748, swap_incr: 114210
iters_decr: 105246, cnt_decr: 52130253, swap_decr: 115524
iters_comp: 110941, cnt_comp: 1141498, swap_comp: 227270
```
- размер 10000:
```bash
Rwlock size: 10000 -- on: 10 seconds
iters_incr: 14794, cnt_incr: 73096510, swap_incr: 15462
iters_decr: 15085, cnt_decr: 74634562, swap_decr: 15406
iters_comp: 15950, cnt_comp: 1737703, swap_comp: 31817
```
- размер 100000:
```bash
Rwlock size: 100000 -- on: 10 seconds
iters_incr: 1194, cnt_incr: 59260034, swap_incr: 1429
iters_decr: 1195, cnt_decr: 58963216, swap_decr: 1432
iters_comp: 1338, cnt_comp: 1372544, swap_comp: 2789
```

## Отличия и объяснения
Так же видно, что количество перестановок растет до размера очереди 10000, а на очереди размера 100000 количество перестановок уже меньше, так как потоку перестановщику дольше идти до места предполагаемой перестановки. Именно поэтому с ростом размера очереди уменьшается и число итераций циклов проверок.

Так же можно заметить, что быстрее всех оказался спинлок, так как секция блокировки спинлоком у нас короткая: либо проверка на длину строк, либо перестновка элементов списка. Мьютекс оказался самым медленным, поскольку при недоступности блокировки/разблокировки мьютекса, он будеть уходить в сон через futex, переключая контекст, когда спинлок будет просто крутиться в цикле в user-space. rwlock же оказался на маленькой и большой очереди схож по скорости с мьютексом, а на средних даже быстрее. Сам rwlock эффективен, когда читателей большей, чем писателей - у нас же и тех, и других одинаковое количество. rwlock также устроен иначе в отличие от других используемых примитивов: несколько читателей могут читать область незаблокированную писателем. На маленькой очереди rwlock и мьютекс примерно одинаковы по скорости, потому что мы часто попадаем на заблокированные области. На средних очередях rwlock обгоняет мьютекс за счет бонуса с читателями. На большой очереди примитивы опять имеют схожую скорость, так как блокировки/разблокировки rwlock-а медленее чем у мьютекса.
