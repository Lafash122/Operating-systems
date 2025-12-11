Реализация односвязного списка, хранящего строки длиной менее 100 символов, у которого с каждым элементом связан отдельный примитив синхронизации: спинлок, мьютекс, блокировка чтения-записи
## Замеры
### Мьютекс
- размер 100:
```bash
Mutex size: 100 -- on: 10 seconds
iters_incr: 1024164, cnt_incr: 44039461, swap_incr: 26
iters_decr: 1021129, cnt_decr: 54119579, swap_decr: 46
iters_comp: 1026975, cnt_comp: 1026955, swap_comp: 41
```
- размер 1000:
```bash
Mutex size: 1000 -- on: 10 seconds
iters_incr: 150758, cnt_incr: 75377159, swap_incr: 160
iters_decr: 150756, cnt_decr: 73571577, swap_decr: 122
iters_comp: 150746, cnt_comp: 1356129, swap_comp: 566
```
- размер 10000:
```bash
Mutex size: 10000 -- on: 10 seconds
iters_incr: 14374, cnt_incr: 73908490, swap_incr: 3815
iters_decr: 14373, cnt_decr: 68477805, swap_decr: 2926
iters_comp: 14373, cnt_comp: 1309404, swap_comp: 6827
```
- размер 100000:
```bash
Mutex size: 100000 -- on: 10 seconds
iters_incr: 1213, cnt_incr: 60131139, swap_incr: 1750
iters_decr: 1212, cnt_decr: 59825031, swap_decr: 1748
iters_comp: 1212, cnt_comp: 1292189, swap_comp: 3513
```

### Спинлок
- размер 100:
```bash
Spinlock size: 100 -- on: 10 seconds
iters_incr: 2056267, cnt_incr: 59634537, swap_incr: 19
iters_decr: 2056696, cnt_decr: 137796664, swap_decr: 159
iters_comp: 2051202, cnt_comp: 6153405, swap_comp: 31
```
- размер 1000:
```bash
Spinlock size: 1000 -- on: 10 seconds
iters_incr: 381240, cnt_incr: 178425608, swap_incr: 160
iters_decr: 381195, cnt_decr: 198216207, swap_decr: 323
iters_comp: 381064, cnt_comp: 3429656, swap_comp: 538
```
- размер 10000:
```bash
Spinlock size: 10000 -- on: 10 seconds
iters_incr: 34707, cnt_incr: 174399020, swap_incr: 4095
iters_decr: 34714, cnt_decr: 169323713, swap_decr: 3778
iters_comp: 34769, cnt_comp: 3352727, swap_comp: 9461
```
- размер 100000:
```bash
Spinlock size: 100000 -- on: 10 seconds
iters_incr: 1986, cnt_incr: 98071520, swap_incr: 3292
iters_decr: 1996, cnt_decr: 99012842, swap_decr: 3370
iters_comp: 2083, cnt_comp: 2107711, swap_comp: 6685
```

### RWlock
- размер 100:
```bash
Rwlock size: 100 -- on: 10 seconds
iters_incr: 794573, cnt_incr: 45290254, swap_incr: 19
iters_decr: 824635, cnt_decr: 33810578, swap_decr: 10
iters_comp: 1663947, cnt_comp: 0, swap_comp: 27
```
- размер 1000:
```bash
Rwlock size: 1000 -- on: 10 seconds
iters_incr: 186413, cnt_incr: 102331566, swap_incr: 408
iters_decr: 191414, cnt_decr: 83276112, swap_decr: 132
iters_comp: 404214, cnt_comp: 4445159, swap_comp: 489
```
- размер 10000:
```bash
Rwlock size: 10000 -- on: 10 seconds
iters_incr: 20395, cnt_incr: 106371655, swap_incr: 5049
iters_decr: 20804, cnt_decr: 97750237, swap_decr: 3036
iters_comp: 36081, cnt_comp: 2936287, swap_comp: 8915
```
- размер 100000:
```bash
Rwlock size: 100000 -- on: 10 seconds
iters_incr: 1156, cnt_incr: 57255573, swap_incr: 1630
iters_decr: 1164, cnt_decr: 57573793, swap_decr: 1556
iters_comp: 1296, cnt_comp: 1307085, swap_comp: 3169
```

## Объяснение
