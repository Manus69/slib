#ifndef _SLIB_H
#define _SLIB_H

#include <stdbool.h>

#define NO_IDX  (-1)
#define $drf(T) * (T *)
#define $nl     printf("\n");
#define $min(x, y) \
    ((x) < (y) ? (x) : (y))
#define $max(x, y) \
    ((x) > (y) ? (x) : (y))
#define $swap(T, x, y) \
{ \
    T _t = $drf(T) x; \
    $drf(T) x = $drf(T) y; \
    $drf(T) y = _t; \
}

#define $cmpf_gen(T) \
static inline int T ## _cmpf(const void * lhs, const void * rhs) \
{ \
    return $drf(T) lhs < $drf(T) rhs ? -1 : $drf(T) lhs > $drf(T) rhs; \
}

#define $swapf_gen(T) \
static inline void T ## _swapf(void * lhs, void * rhs) \
{ \
    $swap(T, lhs, rhs) \
}

#define $hashf_gen(T) \
static inline u64 T ## _hashf(const void * item) \
{ \
    return hash_djb(item, sizeof(T)); \
}

#define $eqf_gen(T) \
static inline bool T ## _eqf(const void * lhs, const void * rhs) \
{ \
    return $drf(T) lhs == $drf(T) rhs; \
}

typedef __INT64_TYPE__  i64;
typedef __INT32_TYPE__  i32;
typedef __UINT64_TYPE__ u64;
typedef unsigned char   byte;

typedef int  (* cmpf)(void const *, void const *);
typedef void (* swapf)(void *, void *);
typedef bool (* eqf)(void const *, void const *);
typedef u64  (* hashf)(void const *);

typedef struct Seg      Seg;
typedef struct Bfd      Bfd;
typedef struct Vec      Vec;
typedef struct Deq      Deq;
typedef struct Vec      Heap;
typedef struct Htbl     Htbl;

struct Seg
{
    void *  ptr;
    i32     len;
    i32     isize;
};

struct Bfd
{
    u64 *   cells;
    i64     len;
};

struct Vec
{
    Seg     seg;
    i32     idx;
};

struct Deq
{
    Seg seg;
    i32 idx;
    i32 len;
};

struct Htbl
{
    Bfd index;
    Bfd rem;
    Seg seg;
    i32 count;
};

static inline u64 hash_djb(const byte * bytes, i32 len)
{
    u64 hash = 5381;

    for (i32 k = 0; k < len; k ++)
    {
        hash = ((hash << 5) + hash) + bytes[k];
    }

    return hash;
}

static inline u64 rng_xor(u64 * seed)
{
    u64 x;

    x = * seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;

    return * seed = x;
}

u64 math_next_pow2(u64 x);
u64 math_ack(u64 m, u64 n);

i32 byte_bits_cstr(byte bt, char * cstr);

//u64
bool u64_bit(u64 x, i32 k);
u64 u64_set_bit(u64 x, i32 k);
u64 u64_clear_bit(u64 x, i32 k);
u64 u64_toggle_bit(u64 x, i32 k);
i32 u64_bits_cstr(u64 x, i32 nbits, char * cstr);
i32 u64_first1(u64 x);
i32 u64_first0(u64 x);

//bfd
Bfd Bfd_ctr(u64 * mem, i64 len);
i32 Bfd_nbits(const Bfd * bfd);
bool Bfd_bit(const Bfd * bfd, i32 k);
void Bfd_set(const Bfd * bfd, i32 k);
void Bfd_clear(const Bfd * bfd, i32 k);
void Bfd_toggle(const Bfd * bfd, i32 k);
void Bfd_zeroes(const Bfd * bfd);
void Bfd_ones(const Bfd * bfd);
bool Bfd_resize(Bfd * bfd, i32 len, bool extend_ones);
bool Bfd_new(Bfd * bfd, i32 nbits);
void Bfd_del(Bfd * bfd);
i32 Bfd_cstr(const Bfd * bfd, char * cstr);


//Seg
Seg Seg_ctr(void * ptr, i32 isize, i32 len);
Seg Seg_sub(const Seg * seg, i32 idx, i32 len);
bool Seg_new(Seg * seg, i32 isize, i32 len);
bool Seg_resize(Seg * seg, i32 len);
bool Seg_extend(Seg * seg, i32 len);
void Seg_del(Seg * seg);
i32 Seg_len(const Seg * seg);
i32 Seg_isize(const Seg * seg);
void Seg_memmove(Seg * seg, i32 from, i32 to, i32 len);
void * Seg_get(const Seg * seg, i32 idx);
void * Seg_first(const Seg * seg);
void * Seg_last(const Seg * seg);
void Seg_set(const Seg * seg, i32 idx, const void * item);
void Seg_map_vv(const Seg * seg, void (* f)(void *));

//Vec
#define $Vec_new(vec, T) Vec_new(& vec, sizeof(T))

Vec Vec_ctr(void * mem, i32 isize, i32 capacity);
bool Vec_new_capacity(Vec * vec, i32 isize, i32 capacity);
bool Vec_new(Vec * vec, i32 isize);
void Vec_del(Vec * vec);
void * Vec_get(const Vec * vec, i32 idx);
void * Vec_first(const Vec * vec);
void * Vec_last(const Vec * vec);
void Vec_set(const Vec * vec, i32 idx, const void * item);
bool Vec_reserve(Vec * vec, i32 capacity);
void Vec_push(Vec * vec, const void * item);
bool Vec_push_check(Vec * vec, const void * item);
void * Vec_pop(Vec * vec);
void * Vec_pop_all(Vec * vec);
i32 Vec_len(const Vec * vec);
i32 Vec_isize(const Vec * vec);
void Vec_map_vv(const Vec * vec, void (* f) (void *));

//Heap
#define $Heap_new(heap, T) Heap_new(& heap, sizeof(T))
bool Heap_new_capacity(Heap * heap, i32 isize, i32 capacity);
bool Heap_new(Heap * heap, i32 isize);
void Heap_del(Heap * heap);
i32 Heap_count(const Heap * heap);
bool Heap_empty(const Heap * heap);
bool Heap_insert_check(Heap * heap, const void * item, cmpf cmp, swapf swap);
void * Heap_top(const Heap * heap);
void * Heap_pop_top(Heap * heap, cmpf cmp, swapf swap);
void * Heap_pop_all(Heap * heap);

//Deq
bool Deq_new(Deq * deq, i32 isize);
void Deq_del(Deq * deq);
i32 Deq_len(const Deq * deq);
i32 Deq_isize(const Deq * deq);
i32 Deq_fcl(const Deq * deq);
i32 Deq_fcr(const Deq * deq);
void * Deq_get(const Deq * deq, i32 idx);
void * Deq_first(const Deq * deq);
void * Deq_last(const Deq * deq);
void Deq_set(const Deq * deq, i32 idx, const void * item);
void Deq_pushl(Deq * deq, const void * item);
void Deq_pushr(Deq * deq, const void * item);
bool Deq_reserver(Deq * deq, i32 len);
bool Deq_reservel(Deq * deq, i32 len);
bool Deq_pushl_check(Deq * deq, const void * item);
bool Deq_pushr_check(Deq * deq, const void * item);
void Deq_map_vv(const Deq * deq, void (* f)(void *));

//Htbl
bool Htbl_new_capacity(Htbl * htbl, i32 isize, i32 capacity);
bool Htbl_new(Htbl * htbl, i32 isize);
void Htbl_del(Htbl * htbl);
void Htbl_purge(Htbl * htbl);
i32 Htbl_count(const Htbl * htbl);
void * Htbl_get(const Htbl * htbl, const void * item, hashf hash, eqf eq);
bool Htbl_insert_check(Htbl * htbl, const void * item, hashf hash);


//io

i32 io_get_line(char * cstr, i32 max_len);

#ifdef _SLIB_IMPL

#include <stdlib.h>
#include <assert.h>
#include <string.h>

// math

u64 math_next_pow2(u64 x)
{
    if (x) return 1ul << (sizeof(x) * __CHAR_BIT__ - __builtin_clzl(x));

    return 1ul;
}

u64 math_ack(u64 m, u64 n)
{
    if (m == 0) return n + 1;
    if (n == 0) return math_ack(m - 1, 1);

    return math_ack(m - 1, math_ack(m, n - 1));
}

// bits

bool u64_bit(u64 x, i32 k)
{
    return x & (1ul << k);
}

u64 u64_set_bit(u64 x, i32 k)
{
    return x | (1ul << k);
}

u64 u64_clear_bit(u64 x, i32 k)
{
    return x & ~(1ul << k);
}

u64 u64_toggle_bit(u64 x, i32 k)
{
    return x ^ (1ul << k);
}

i32 u64_first1(u64 x)
{
    if (x) return __builtin_ctzll(x);

    return NO_IDX;
}

i32 u64_first0(u64 x)
{
    return u64_first1(~ x);
}

i32 byte_bits_cstr(byte bt, char * cstr)
{
    char    symbols[] = {'0', '1'};

    for (i32 k = 0; k < __CHAR_BIT__; k ++)
    {
        cstr[k] = symbols[bt & (1 << k)];
    }

    return __CHAR_BIT__;
}

i32 u64_bits_cstr(u64 x, i32 nbits, char * cstr)
{
    char    symbols[] = {'0', '1'};
    i32     k;

    for (k = 0; k < nbits; k ++)
    {
        cstr[k] = symbols[u64_bit(x, k)];
    }

    return k;
}

// Bfd
#define _BFD_CELL_SIZE 64

static i32 _Bfd_nbits_to_ncells(i32 nbits)
{
    return nbits / _BFD_CELL_SIZE + (nbits % _BFD_CELL_SIZE != 0);
}

Bfd Bfd_ctr(u64 * mem, i64 len)
{
    return (Bfd)
    {
        .cells = mem,
        .len = len,
    };
}

i32 Bfd_nbits(const Bfd * bfd)
{
    return bfd->len * _BFD_CELL_SIZE;
}

bool Bfd_bit(const Bfd * bfd, i32 k)
{
    return u64_bit(bfd->cells[k / _BFD_CELL_SIZE], k % _BFD_CELL_SIZE);
}

void Bfd_set(const Bfd * bfd, i32 k)
{
    bfd->cells[k / _BFD_CELL_SIZE] = u64_set_bit(bfd->cells[k / _BFD_CELL_SIZE], k % _BFD_CELL_SIZE);
}

void Bfd_clear(const Bfd * bfd, i32 k)
{
    bfd->cells[k / _BFD_CELL_SIZE] = u64_clear_bit(bfd->cells[k / _BFD_CELL_SIZE], k % _BFD_CELL_SIZE);
}

void Bfd_toggle(const Bfd * bfd, i32 k)
{
    bfd->cells[k / _BFD_CELL_SIZE] = u64_toggle_bit(bfd->cells[k / _BFD_CELL_SIZE], k % _BFD_CELL_SIZE);
}

static void _Bfd_memset(const Bfd * bfd, i32 idx, i32 len, u64 val)
{
    for (i32 k = 0; k < len; k ++)
    {
        bfd->cells[k + idx] = val;
    }
}

void Bfd_zeroes(const Bfd * bfd)
{
    _Bfd_memset(bfd, 0, bfd->len, 0);
}

void Bfd_ones(const Bfd * bfd)
{
    _Bfd_memset(bfd, 0, bfd->len, (u64) (-1));
}

bool Bfd_resize(Bfd * bfd, i32 len, bool extend_ones)
{
    if ((bfd->cells = realloc(bfd->cells, len * sizeof(u64))))
    {
        _Bfd_memset(bfd, bfd->len, len - bfd->len, extend_ones ? -1 : 0);
        bfd->len = len;

        return true;
    }

    return false;
}

bool Bfd_new(Bfd * bfd, i32 nbits)
{
    bfd->len = _Bfd_nbits_to_ncells(nbits);
    bfd->cells = calloc(bfd->len, sizeof(u64));

    return bfd->cells;
}

void Bfd_del(Bfd * bfd)
{
    free(bfd->cells);
}

i32 Bfd_cstr(const Bfd * bfd, char * cstr)
{
    i32 len;
    i32 current;

    len = 0;
    for (i32 k = 0; k < bfd->len; k ++)
    {
        current = u64_bits_cstr(bfd->cells[k], _BFD_CELL_SIZE, cstr);
        cstr += current;
        len += current;
    }

    return len;
}

#undef _BFD_CELL_SIZE

//Seg

Seg Seg_ctr(void * ptr, i32 isize, i32 len)
{
    return (Seg)
    {
        .ptr = ptr,
        .len = len,
        .isize = isize,
    };
}

Seg Seg_sub(const Seg * seg, i32 idx, i32 len)
{
    return Seg_ctr(Seg_get(seg, idx), seg->isize, len);
}

bool Seg_new(Seg * seg, i32 isize, i32 len)
{
    * seg = Seg_ctr(calloc(len, isize), isize, len);

    return seg->ptr;
}

bool Seg_resize(Seg * seg, i32 len)
{
    seg->ptr = realloc(seg->ptr, len * seg->isize);
    seg->len = len;

    return seg->ptr;
}

bool Seg_extend(Seg * seg, i32 len)
{
    return Seg_resize(seg, len + seg->len);
}

void Seg_memmove(Seg * seg, i32 from, i32 to, i32 len)
{
    memmove(Seg_get(seg, to), Seg_get(seg, from), len * seg->isize);
}

void Seg_del(Seg * seg)
{
    free(seg->ptr);
}

i32 Seg_len(const Seg * seg)
{
    return seg->len;
}

i32 Seg_isize(const Seg * seg)
{
    return seg->isize;
}

void * Seg_get(const Seg * seg, i32 idx)
{
    return seg->ptr + idx * seg->isize;
}

void * Seg_first(const Seg * seg)
{
    return Seg_get(seg, 0);
}

void * Seg_last(const Seg * seg)
{
    return Seg_get(seg, seg->len - 1);
}

void Seg_set(const Seg * seg, i32 idx, const void * item)
{
    memcpy(Seg_get(seg, idx), item, seg->isize);
}

void Seg_map_vv(const Seg * seg, void (* f)(void *))
{
    for (i32 k = 0; k < seg->len; k ++)
    {
        f(Seg_get(seg, k));
    }
}


// Vec
#define _VEC_DC (1 << 4)

Vec Vec_ctr(void * mem, i32 isize, i32 capacity)
{
    return (Vec)
    {
        .seg = Seg_ctr(mem, isize, capacity),
        .idx = 0,
    };
}

bool Vec_new_capacity(Vec * vec, i32 isize, i32 capacity)
{
    vec->idx = 0;

    return Seg_new(& vec->seg, isize, capacity);
}

bool Vec_new(Vec * vec, i32 isize)
{
    return Vec_new_capacity(vec, isize, _VEC_DC);
}

void Vec_del(Vec * vec)
{
    Seg_del(& vec->seg);
}

i32 Vec_len(const Vec * vec)
{
    return vec->idx;
}

i32 Vec_isize(const Vec * vec)
{
    return Seg_isize(& vec->seg);
}

void * Vec_get(const Vec * vec, i32 idx)
{
    return Seg_get(& vec->seg, idx);
}

void * Vec_first(const Vec * vec)
{
    return Seg_first(& vec->seg);
}

void * Vec_last(const Vec * vec)
{
    return Vec_get(vec, vec->idx - 1);
}

void Vec_set(const Vec * vec, i32 idx, const void * item)
{
    Seg_set(& vec->seg, idx, item);
}

bool Vec_reserve(Vec * vec, i32 capacity)
{
    if (vec->idx + capacity > Seg_len(& vec->seg))
    {
        return Seg_resize(& vec->seg, math_next_pow2(capacity + vec->idx));
    }

    return true;
}

void Vec_push(Vec * vec, const void * item)
{
    Vec_set(vec, vec->idx, item);
    vec->idx ++;
}

bool Vec_push_check(Vec * vec, const void * item)
{
    if (Vec_reserve(vec, 1))
    {
        Vec_push(vec, item);
        return true;
    }

    return false;
}

void * Vec_pop(Vec * vec)
{
    return Vec_get(vec, -- vec->idx);
}

void * Vec_pop_all(Vec * vec)
{
    vec->idx = 0;

    return Vec_first(vec);
}

void Vec_map_vv(const Vec * vec, void (* f) (void *))
{
    Seg seg;

    seg = Seg_ctr(Vec_first(vec), Vec_isize(vec), Vec_len(vec));
    Seg_map_vv(& seg, f);
}

#undef _VEC_DC


//Heap
static i32 _Heap_parent_idx(i32 k)
{
    return (k - 1) / 2;
}

static i32 _Heap_left_idx(i32 k)
{
    return k * 2 + 1;
}

static i32 _Heap_right_idx(i32 k)
{
    return k * 2 + 2;
}

static void * _Heap_end(Heap * heap)
{
    return Vec_get(heap, Vec_len(heap));
}

static bool _Heap_idx_valid(const Heap * heap, i32 idx)
{
    return idx < Vec_len(heap);
}

static int _Heap_cmp(const Heap * heap, i32 lhs, i32 rhs, cmpf cmp)
{
    return cmp(Vec_get(heap, lhs), Vec_get(heap, rhs));
}

static void _Heap_swap(const Heap * heap, i32 lhs, i32 rhs, swapf swap)
{
    swap(Vec_get(heap, lhs), Vec_get(heap, rhs));
}

bool Heap_new_capacity(Heap * heap, i32 isize, i32 capacity)
{
    return Vec_new_capacity(heap, isize, capacity);
}

bool Heap_new(Heap * heap, i32 isize)
{
    return Vec_new(heap, isize);
}

void Heap_del(Heap * heap)
{
    Vec_del(heap);
}

i32 Heap_count(const Heap * heap)
{
    return Vec_len(heap);
}

bool Heap_empty(const Heap * heap)
{
    return Heap_count(heap) == 0;
}

static void _Heap_propagate_up(Heap * heap, i32 idx, cmpf cmp, swapf swap)
{
    i32 parent_idx;

    if (idx == 0) return ;

    parent_idx = _Heap_parent_idx(idx);

    if (_Heap_cmp(heap, parent_idx, idx, cmp) > 0)
    {
        _Heap_swap(heap, parent_idx, idx, swap);

        return _Heap_propagate_up(heap, parent_idx, cmp, swap);
    }
}

static i32 _Heap_min_idx(const Heap * heap, i32 idx, cmpf cmp)
{
    i32 left_idx, right_idx, min_idx;

    left_idx = _Heap_left_idx(idx);
    right_idx = _Heap_right_idx(idx);

    if (! _Heap_idx_valid(heap, left_idx))
    {
        min_idx = idx;
    }
    else if (! _Heap_idx_valid(heap, right_idx))
    {
        min_idx = _Heap_cmp(heap, left_idx, idx, cmp) < 0 ? left_idx : idx; 
    }
    else
    {
        min_idx = _Heap_cmp(heap, left_idx, right_idx, cmp) < 0 ? left_idx : right_idx;
        min_idx = _Heap_cmp(heap, min_idx, idx, cmp) < 0 ? min_idx : idx;
    }

    return min_idx;
}

static void _Heap_propagate_down(Heap * heap, i32 idx, cmpf cmp, swapf swap)
{
    i32 min_idx;

    min_idx = _Heap_min_idx(heap, idx, cmp);
    if (min_idx != idx)
    {
        swap(Vec_get(heap, min_idx), Vec_get(heap, idx));

        _Heap_propagate_down(heap, min_idx, cmp, swap);
    }
}

bool Heap_insert_check(Heap * heap, const void * item, cmpf cmp, swapf swap)
{
    if (! Vec_push_check(heap, item)) return false;

    _Heap_propagate_up(heap, Vec_len(heap) - 1, cmp, swap);

    return true;
}

void * Heap_top(const Heap * heap)
{
    return Vec_first(heap);
}

void * Heap_pop_top(Heap * heap, cmpf cmp, swapf swap)
{
    if (Heap_empty(heap)) return 0;
    if (Heap_count(heap) == 1) return Vec_pop(heap);

    Vec_pop(heap);
    _Heap_swap(heap, 0, Heap_count(heap), swap);
    _Heap_propagate_down(heap, 0, cmp, swap);

    return _Heap_end(heap);
}

void * Heap_pop_all(Heap * heap)
{
    return Vec_pop_all(heap);
}

// Deq
#define _DEQ_DC (1 << 4)

bool Deq_new(Deq * deq, i32 isize)
{
    Seg_new(& deq->seg, isize, _DEQ_DC);
    deq->idx = _DEQ_DC / 2;
    deq->len = 0;

    return Seg_first(& deq->seg);
}

void Deq_del(Deq * deq)
{
    Seg_del(& deq->seg);
}

i32 Deq_len(const Deq * deq)
{
    return deq->len;
}

i32 Deq_isize(const Deq * deq)
{
    return Seg_isize(& deq->seg);
}

i32 Deq_fcl(const Deq * deq)
{
    return deq->idx - Deq_len(deq);
}

i32 Deq_fcr(const Deq * deq)
{
    return Seg_len(& deq->seg) - deq->idx;
}

void * Deq_get(const Deq * deq, i32 idx)
{
    return Seg_get(& deq->seg, deq->idx - deq->len + idx);
}

void * Deq_first(const Deq * deq)
{
    return Deq_get(deq, 0);
}

void * Deq_last(const Deq * deq)
{
    return Deq_get(deq, Deq_len(deq) - 1);
}

void Deq_set(const Deq * deq, i32 idx, const void * item)
{
    memcpy(Deq_get(deq, idx), item, Seg_isize(& deq->seg));
}

void Deq_pushl(Deq * deq, const void * item)
{
    Deq_set(deq, -1, item);
    deq->len ++;
}

void Deq_pushr(Deq * deq, const void * item)
{
    deq->len ++;

    Vec_push((Vec *) deq, item);
}

bool Deq_reserver(Deq * deq, i32 len)
{
    return Vec_reserve((Vec *) deq, len);
}

bool Deq_reservel(Deq * deq, i32 len)
{
    i32 idx;

    if (Deq_fcl(deq) >= len) return true;
    if (! Seg_resize(& deq->seg, math_next_pow2(Seg_len(& deq->seg) + len))) return false;

    idx = Seg_len(& deq->seg) / 2;

    memmove(Seg_get(& deq->seg, idx), Deq_first(deq), Deq_len(deq) * Deq_isize(deq));
    deq->idx = idx + Deq_len(deq);

    return true;
}

bool Deq_pushl_check(Deq * deq, const void * item)
{
    if (Deq_reservel(deq, 1))
    {
        Deq_pushl(deq, item);

        return true;
    }

    return false;
}

bool Deq_pushr_check(Deq * deq, const void * item)
{
    if (Deq_reserver(deq, 1))
    {
        Deq_pushr(deq, item);

        return true;
    }

    return false;
}

void Deq_map_vv(const Deq * deq, void (* f)(void *))
{
    Seg seg;

    seg = Seg_ctr(Deq_first(deq), Deq_isize(deq), Deq_len(deq));
    Seg_map_vv(& seg, f);
}

#undef _DEQ_DC

#define _HTBL_DC    (1 << 6)
#define _HTBL_LOADF (0.75)

bool Htbl_new_capacity(Htbl * htbl, i32 isize, i32 capacity)
{
    * htbl = (Htbl) {};

    return (Bfd_new(& htbl->index, capacity) && 
            Bfd_new(& htbl->rem, capacity) && 
            Seg_new(& htbl->seg, isize, capacity));
}

bool Htbl_new(Htbl * htbl, i32 isize)
{
    return Htbl_new_capacity(htbl, isize, _HTBL_DC);
}

void Htbl_del(Htbl * htbl)
{
    Bfd_del(& htbl->index);
    Bfd_del(& htbl->rem);
    Seg_del(& htbl->seg);
}

i32 Htbl_count(const Htbl * htbl)
{
    return htbl->count;
}

static void * _Htbl_get(const Htbl * htbl, i32 idx)
{
    return Seg_get(& htbl->seg, idx);
}

static bool _Htbl_has(const Htbl * htbl, i32 idx)
{
    return Bfd_bit(& htbl->index, idx);
}

static bool _Htbl_had(const Htbl * htbl, i32 idx)
{
    return Bfd_bit(& htbl->rem, idx);
}

static i32 _Htbl_hash_idx(const Htbl * htbl, const void * item, hashf hash)
{
    u64 val;

    val = hash(item);
    val = val % Seg_len(& htbl->seg);

    return val;
}

static i32 _Htbl_next_empty(const Htbl * htbl, i32 idx)
{
    while (true)
    {
        if (! _Htbl_has(htbl, idx)) return idx;

        idx = (idx + 1) % Seg_len(& htbl->seg);
    }
}

static double _Htbl_load(const Htbl * htbl)
{
    return ((double) htbl->count) / Seg_len(& htbl->seg);
}

static i32 _Htbl_idxof(const Htbl * htbl, const void * item, i32 start, eqf eq)
{
    while (true)
    {
        if (_Htbl_has(htbl, start))
        {
            if (eq(_Htbl_get(htbl, start), item)) return start;
        }
        else if (! _Htbl_had(htbl, start)) return NO_IDX;

        start = (start + 1) % Seg_len(& htbl->seg);
    }
}

void * Htbl_get(const Htbl * htbl, const void * item, hashf hash, eqf eq)
{
    i32 idx;

    if (! Htbl_count(htbl)) return 0;

    idx = _Htbl_hash_idx(htbl, item, hash);
    idx = _Htbl_idxof(htbl, item, idx, eq);

    return idx == NO_IDX ? 0 : _Htbl_get(htbl, idx);
}

static void _Htbl_insert(Htbl * htbl, i32 idx, const void * item)
{
    Seg_set(& htbl->seg, idx, item);
    Bfd_set(& htbl->index, idx);

    htbl->count ++;
}

static bool _Htbl_rehash(Htbl * htbl, i32 capacity, hashf hash)
{
    Htbl    new;
    void *  item;
    i32     idx;

    if (! Htbl_new_capacity(& new, Seg_isize(& htbl->seg), capacity)) return false;
    
    for (i32 k = 0; k < Seg_len(& htbl->seg); k ++)
    {
        if (_Htbl_has(htbl, k))
        {
            item = _Htbl_get(htbl, k);
            idx = _Htbl_hash_idx(& new, item, hash);
            idx = _Htbl_next_empty(& new, idx);

            _Htbl_insert(& new, idx, item);
        }
    }

    Htbl_del(htbl);
    * htbl = new;

    return true;
}

bool Htbl_insert_check(Htbl * htbl, const void * item, hashf hash)
{
    i32 idx;

    if (_Htbl_load(htbl) > _HTBL_LOADF)
    {
        if (! _Htbl_rehash(htbl, Seg_len(& htbl->seg) * 2, hash)) return false;
    }

    idx = _Htbl_hash_idx(htbl, item, hash);
    idx = _Htbl_next_empty(htbl, idx);

    _Htbl_insert(htbl, idx, item);

    return true;
}

void Htbl_purge(Htbl * htbl)
{
    htbl->count = 0;
    Bfd_zeroes(& htbl->index);
    Bfd_zeroes(& htbl->rem);
}

#undef _HTBL_DC
#undef _HTBL_LOADF

// io
#include <stdio.h>

i32 io_get_line(char * cstr, i32 max_len)
{
    int x;

    for (i32 len = 0; len < max_len; len ++)
    {
        x = getc(stdin);
        if (x == '\n')  return len;
        if (x == EOF)   return len;

        * cstr ++ = x;
    }

    return NO_IDX;
}



#endif
#endif