#include "li_value.h"
#include "li_debug.h"
#include "li_mem.h"
#include "li_util.h"
#include <string.h>

void printValue(Value v){
    
    switch(v){
        case FALSE_VAL:
            printf("false\n");
            break;
        case TRUE_VAL:
            printf("true\n");
            break;
        case NIL_VAL:
            printf("null\n");
            break;
        case UNDEF_VAL:
            printf("undefined\n");
            break;
        default:
            if(IS_NUM(v)){
                ValueBit vb;
                vb.value = v;
                double intpart;
                if(modf(vb.asDouble, &intpart) == 0){
                    printf("%ld\n", (int64_t)intpart);
                }
                else{
                    printf("%f\n", vb.asDouble);
                }
            }else{
                printf("haven't implemented\n");
            }
    }
}

Value liNumNeg(Value v){
    ASSERT(IS_NUM(v), "should be passed a number value");
    return numToValue(- valueToNum(v));
}
Value liNumAdd(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) + valueToNum(r));
}
Value liNumMinus(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) - valueToNum(r));
}
Value liNumMulply(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) * valueToNum(r));
}
Value liNumDiv(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(valueToNum(l) / valueToNum(r));
}
Value numPow(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    return numToValue(pow(valueToNum(l), valueToNum(r)));
}
Value liNumIDiv(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    double res;
    modf(valueToNum(l) / valueToNum(r), &res);
    return numToValue(res);
}
Value liNumMod(Value l, Value r){
    ASSERT(IS_NUM(l) && IS_NUM(r), "should be passed a number value");
    int64_t v1 = (int64_t) valueToNum(l);
    int64_t v2 = (int64_t) valueToNum(r);
    return numToValue((double) (v1%v2));
}
Value liBitNeg(Value v){
    ValueBit vb;
    vb.value = v;
    ASSERT(IS_NUM(v) && modf(vb.asDouble, &vb.asDouble) == 0 , "should be passed an interger number value");
    uint64_t tmp = (uint64_t) vb.asDouble;
    vb.asDouble = (double) (~ tmp);
    return vb.value;
}
Value liBitAnd(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 & v2);
    return vb1.value;
}
Value liBitOr(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 | v2);
    return vb1.value;
}
Value liBitXor(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 ^ v2);
    return vb1.value;
}
Value liBitShiftL(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    uint64_t v1 = (uint64_t) valueToNum(l), v2 = (uint64_t) valueToNum(r);
    return numToValue((double)(v1 << v2));
}
Value liBitShiftR(Value l, Value r){
    ValueBit vb1, vb2;
    vb1.value = l, vb2.value = r;
    ASSERT(IS_NUM(l) && IS_NUM(r) && modf(vb1.asDouble, &vb1.asDouble) == 0  && modf(vb2.asDouble, &vb2.asDouble) == 0 , "should be passed an interger number value");
    vb1.value = l;vb2.value = r;
    uint64_t v1 = (uint64_t) vb1.asDouble, v2 = (uint64_t) vb2.asDouble;
    vb1.asDouble = (double)(v1 >> v2);
    return vb1.value;
}

Value liBoolAnd(Value l, Value r){
    Value v1 = (IS_NIL(l) || IS_FALSE(l))?FALSE_VAL:TRUE_VAL;
    Value v2 = (IS_NIL(r) || IS_FALSE(r))?FALSE_VAL:TRUE_VAL;
    return (IS_TRUE(v1) && IS_TRUE(v2))?TRUE_VAL:FALSE_VAL;
}
Value liBoolOr(Value l, Value r){
    Value v1 = (IS_NIL(l) || IS_FALSE(l))?FALSE_VAL:TRUE_VAL;
    Value v2 = (IS_NIL(r) || IS_FALSE(r))?FALSE_VAL:TRUE_VAL;
    return (IS_TRUE(v1) || IS_TRUE(v2))?TRUE_VAL:FALSE_VAL;
}
Value liBoolXor(Value l, Value r){
    Value v1 = (IS_NIL(l) || IS_FALSE(l))?FALSE_VAL:TRUE_VAL;
    Value v2 = (IS_NIL(r) || IS_FALSE(r))?FALSE_VAL:TRUE_VAL;
    return (v1 != v2)?TRUE_VAL:FALSE_VAL;
}
bool liEqual(Value l, Value r){
    if(IS_NUM(l) && IS_NUM(r)){
        return valueToNum(l) == valueToNum(r);
    }
    else if(!IS_NUM(l) && !IS_NUM(r)){  //both are not number
        if(!IS_OBJ(l) && !IS_OBJ(r)){ //singleon value, just to compare the origin value;
            return l == r;
        }
        else if(IS_OBJ(l) && IS_OBJ(r)){
            if(GET_TAG(l) == GET_TAG(r)){
                switch(GET_TAG(l)){//TODO: ADD compare for internal items
                    case TAG_MAP:
                        return l == r;  
                    case TAG_LIST:
                        return l == r;
                }
            }
        }
    }
    else {
        return false;
    }
    UNREACHABLE();
}

bool liNEqual(Value l, Value r){
    return !liEqual(l,r);
}

ObjString* liStringWithLength(LionVm* vm, size_t len){
    ObjString* str = liNewValueOfType(vm, ObjString);
    str->content = liNewArrayOfType(vm, char, len+1);
    return str;
}

Value liNewString(LionVm* vm, const char* cstring){
    ObjString* str;
    if(cstring){
        size_t length = strlen(cstring);
        str = liStringWithLength(vm, length);
        str->length = length;
        memcpy(str->content, cstring, length);
        str->content[str->length] = '\0';
        str->hash = hashString(str->content);
    }
    else {
        str = liStringWithLength(vm, 0);
        str->hash = 0;
    }
    return objStringToValue(str);
}
Value stringLength(LionVm* vm, ObjString* string){
    ValueBit vb;
    vb.asDouble = string->length;
    return vb.value;
}
Value stringSub(LionVm* vm, ObjString* string, size_t start, size_t end){
    //Todo start == end will meet some problem???
    size_t len = end - start;
    ASSERT(start <= end && end <= string->length, "index out of the length of string");
    ObjString* str = liStringWithLength(vm, len);
    memcpy(str->content, string->content, len);
    return objStringToValue(str);
}
void  printString(Value v){
    printf("%s\n", valueToString(v)->content);
}

Value liNewUpvalue(LionVm* vm);

Value liNewClass(LionVm* vm);
Value liClassBindSuper(LionVm* vm, ObjClass* class, ObjClass* super_class);

DEFINE_BUFFER(Value, Value);

Value liNewList(LionVm* vm, uint32_t size){
    ObjList* list = liNewValueOfType(vm, ObjList);
    liValueBufferInit(&list->elements);
    return objListToValue(list);
}
void liListInsert(LionVm* vm, ObjList* list, Value value, uint32_t index){
    liValueBufferWrite(vm, &list->elements, NIL_VAL);
    for (uint32_t i = list->elements.count - 1; i > index; i--)
    {
        list->elements.data[i] = list->elements.data[i - 1];
    }
    
    list->elements.data[index] = value;
}
void liListRemove(LionVm* vm, ObjList* list, Value index){
    for(uint32_t i = index; i < list->elements.count-1; ++i){
        list->elements.data[i] = list->elements.data[i+1];
    }
    list->elements.data[list->elements.count] = UNDEF_VAL;
    list->elements.count--;
}
Value liListSize(LionVm* vm, ObjList* list){
    return numToValue(list->elements.count);
}

static inline uint32_t hashBits(ValueBit bits) {
  uint32_t result = bits.as2Uint32[0] ^ bits.as2Uint32[1];
  
  // Slosh the bits around some. Due to the way doubles are represented, small
  // integers will have most of low bits of the double respresentation set to
  // zero. For example, the above result for 5 is 43d00600.
  //
  // We map that to an entry index by masking off the high bits which means
  // most small integers would all end up in entry zero. That's bad. To avoid
  // that, push a bunch of the high bits lower down so they affect the lower
  // bits too.
  //
  // The specific mixing function here was pulled from Java's HashMap
  // implementation.
  result ^= (result >> 20) ^ (result >> 12);
  result ^= (result >> 7) ^ (result >> 4);
  return result;
}

static inline uint32_t hashNumber(Value v) {
    ValueBit vb;
    vb.value = v;
    return hashBits(vb);
}

static inline uint32_t calcuHash(Value v){
    if(IS_NUM(v)){
        return hashNumber(v);
    }
    else if (!IS_OBJ(v)) {
        return (uint32_t)(v>>48);
    }
    else {
        switch(GET_TAG(v)){
            case TAG_STRING:
                return valueToString(v)->hash;
            case TAG_CLASS:
                
            case TAG_CLOSURE:
                
            case TAG_UPVALUE:
                
            case TAG_FN:
                
            case TAG_FOREIGN:
                
            case TAG_INSTANCE:
                
            default:
                UNREACHABLE();
        }
    }
}

static void _mapItemsBufferInit(MapItem* buffer, int capacity){
    for(int i = 0; i < capacity; ++i){
        buffer[i].key = UNDEF_VAL;
        buffer[i].value = FALSE_VAL;
    }
}



Value liNewMap(LionVm* vm, uint32_t num_items){
    ObjMap*  map = liNewValueOfType(vm, ObjMap);
    map->capacity = 5;
    map->count = 0;
    map->items = liNewArrayOfType(vm, MapItem, 5);
    _mapItemsBufferInit(map->items, 5);
    return objMapToValue(map);
}

static MapItem* findItemInsertPlace(LionVm* vm, ObjMap* map, Value key){
    ASSERT(map->capacity > 0, "map_capacity should be greater than 0");
    uint32_t hash = calcuHash(key);
    int start = hash%map->capacity, i = start;
    do{
        MapItem* cur_item = &map->items[i];
        if(liEqual(cur_item->key, key)){
            return cur_item;
        }
        else {
            if(cur_item->key == UNDEF_VAL && cur_item->value == FALSE_VAL){
                return cur_item;
            } else {
                i = (i + 1)%map->capacity;
            }
        }
    }while(i != start);
    UNREACHABLE();
}

bool liExistKey(LionVm* vm, ObjMap* map, Value key){
    MapItem* item = findItemInsertPlace(vm, map, key);
    return !IS_UNDEF(item->key);
}
/* use {UNDEF_VAL, FALSE_VAL} to reperesent the empty hole
 * use {UNDEF_VAL, TRUE_VAL} to reperesent the tombstone;
 */

/* this functin will not change the count info
 */
static void _mapItemsBufferInsert(MapItem* buffer, int capacity, Value key, Value value, uint32_t hash){
    int start = hash%capacity, i = start;
    do{
        if(buffer[i].key == UNDEF_VAL){
            buffer[i].key = key;
            buffer[i].value = value;
            buffer[i].hash = hash;
        }
        else{
            if(liEqual(buffer[i].key, key)){
                buffer[i].value = value;
                buffer[i].hash = hash;
            }
            else{
                i = (i + 1) % (capacity);
            }
        }
    }while(i != start);
    UNREACHABLE();
}

static void liResizeMap(LionVm* vm, ObjMap* map, int capacity){
    MapItem* new_buffer = liNewArrayOfType(vm, MapItem, capacity);
    MapItem* old_buffer = map->items;
    _mapItemsBufferInit(new_buffer, capacity);
    for(int i = 0; i < map->capacity; ++i){
        if(!IS_UNDEF(old_buffer[i].key)){
            _mapItemsBufferInsert(new_buffer, capacity, old_buffer[i].key, old_buffer[i].value, old_buffer[i].hash);
        }
    }
}
    
void liMapUpdate(LionVm* vm, ObjMap* map, Value key, Value value){
    int capacity = map->capacity;
    if((double)(map->count + 1)/map->capacity * 100 > LION_MAP_CAPACITY_RATE){
        capacity *= LION_MAP_GROWTH_FACTOR;
        map->capacity = capacity;
        liResizeMap(vm, map, capacity);
    }
    MapItem* item = findItemInsertPlace(vm, map, key);
    if(IS_UNDEF(item->key)){
        ++map->count;
        item->key = key;
        item->hash = calcuHash(key);
    }
    item->value = value;
}

void liMapRemove(LionVm* vm, ObjMap* map, Value key){
    MapItem* item = findItemInsertPlace(vm, map, key);
    if(!IS_UNDEF(item->key)){
        item->key = UNDEF_VAL;
        item->value = TRUE_VAL;
        --map->count;
    }
}


int liMapSize(LionVm* vm, ObjMap* map){
    return map->count;
}

LionVm* liNewVm(){
    LionVm* vm = (LionVm*) malloc(sizeof(LionVm));
    return vm;
}

