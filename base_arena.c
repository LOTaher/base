#include "base.h"
#include <string.h>

Arena* arena_create(U64 capacity)
{
    U64 page_size = mem_page_size();
    capacity = arena_align_forward(capacity, page_size);

    void* base = mem_reserve(capacity);
    if (base == NULL) {
        return NULL;
    }

    U64 initial_commit = arena_align_forward(sizeof(Arena) + ARENA_COMMIT_CHUNK, page_size);
    if (initial_commit > capacity) {
        initial_commit = capacity;
    }

    if (!mem_commit(base, initial_commit)) {
        mem_release(base, capacity);
        return NULL;
    }

    Arena* arena = (Arena*)base;
    arena->capacity  = capacity;
    arena->committed = initial_commit;
    arena->pos       = sizeof(*arena);
    return arena;
}

void arena_destroy(Arena* arena)
{
    mem_release(arena, arena->capacity);
}

U64 arena_align_forward(U64 pos, U64 alignment)
{
    return (pos + (alignment - 1)) & ~(alignment - 1);
}

void* arena_push(Arena* arena, U64 size)
{
    U64 aligned_pos = arena_align_forward(arena->pos, sizeof(void*));
    U64 new_pos = aligned_pos + size;

    if (new_pos > arena->capacity) {
        return NULL;
    }

    if (new_pos > arena->committed) {
        U64 page_size = mem_page_size();
        U64 needed = arena_align_forward(new_pos, page_size);
        U64 grow_to = Max(needed, arena->committed + ARENA_COMMIT_CHUNK);
        grow_to = Min(grow_to, arena->capacity);

        U64 commit_size = grow_to - arena->committed;
        U8* commit_ptr = (U8*)arena + arena->committed;

        if (!mem_commit(commit_ptr, commit_size)) {
            return NULL;
        }
        arena->committed = grow_to;
    }

    arena->pos = new_pos;
    U8* block = (U8*)arena + aligned_pos;
    memset(block, 0, size);
    return block;
}

void arena_clear(Arena* arena)
{
    arena->pos = sizeof(*arena);
}

U64 arena_mark(Arena* arena)
{
    return arena->pos;
}

void arena_pop(Arena* arena, U64 mark)
{
    if (mark < sizeof(*arena)) {
        mark = sizeof(*arena);
    }
    arena->pos = mark;
}

ArenaTemp arena_temp_begin(Arena* arena)
{
    ArenaTemp temp = {0};
    temp.arena = arena;
    temp.pos = arena->pos;
    return temp;
}

void arena_temp_end(ArenaTemp arena)
{
    arena_pop(arena.arena, arena.pos);
}

