#pragma once

#include <util/system/defaults.h>

#include <iosfwd>
#include <vector>

//misc
class TBuffer;

//functors
template <class T = void>
struct TLess;

template <class T = void>
struct TGreater;

template <class T = void>
struct TEqualTo;

template <class T>
struct THash;

//intrusive containers
struct TIntrusiveListDefaultTag;
template <class T, class Tag = TIntrusiveListDefaultTag>
class TIntrusiveList;

template <class T, class D, class Tag = TIntrusiveListDefaultTag>
class TIntrusiveListWithAutoDelete;

template <class T, class Tag = TIntrusiveListDefaultTag>
class TIntrusiveSList;

template <class TValue, class TCmp>
class TRbTree;

//containers
template <class T, class A = std::allocator<T>>
class TDeque;

template <class Key, class T, class HashFcn = THash<Key>, class EqualKey = TEqualTo<Key>, class Alloc = std::allocator<Key>>
class THashMap;

template <class Key, class T, class HashFcn = THash<Key>, class EqualKey = TEqualTo<Key>, class Alloc = std::allocator<Key>>
class THashMultiMap;

template <class Value, class HashFcn = THash<Value>, class EqualKey = TEqualTo<Value>, class Alloc = std::allocator<Value>>
class THashSet;

template <class Value, class HashFcn = THash<Value>, class EqualKey = TEqualTo<Value>, class Alloc = std::allocator<Value>>
class THashMultiSet;

template <class K, class L = TLess<K>, class A = std::allocator<K>>
class TSet;

template <class K, class L = TLess<K>, class A = std::allocator<K>>
class TMultiSet;

template <class T, class S = TDeque<T>>
class TStack;

template <size_t BitCount, typename TChunkType = ui64>
class TBitMap;

//autopointers
class TDelete;
class TDeleteArray;
class TFree;
class TCopyNew;

template <class T, class D = TDelete>
class TAutoPtr;

template <class T, class D = TDelete>
class THolder;

template <class T, class C, class D = TDelete>
class TRefCounted;

template <class T>
class TDefaultIntrusivePtrOps;

template <class T, class Ops>
class TSimpleIntrusiveOps;

template <class T, class Ops = TDefaultIntrusivePtrOps<T>>
class TIntrusivePtr;

template <class T, class Ops = TDefaultIntrusivePtrOps<T>>
class TIntrusiveConstPtr;

template <class T, class Ops = TDefaultIntrusivePtrOps<T>>
using TSimpleIntrusivePtr = TIntrusivePtr<T, TSimpleIntrusiveOps<T, Ops>>;

template <class T, class C, class D = TDelete>
class TSharedPtr;

template <class T, class C = TCopyNew, class D = TDelete>
class TCopyPtr;

template <class TPtr, class TCopy = TCopyNew>
class TCowPtr;

template <typename T>
class TPtrArg;

template <typename T>
using TArrayHolder = THolder<T, TDeleteArray>;

template <typename T>
using TMallocHolder = THolder<T, TFree>;

template <typename T>
using TArrayPtr = TAutoPtr<T, TDeleteArray>;

template <typename T>
using TMallocPtr = TAutoPtr<T, TFree>;

struct TGUID;

template <class T>
class TArrayRef;

template <class T>
using TConstArrayRef = TArrayRef<const T>;
