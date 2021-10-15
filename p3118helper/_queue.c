#include <Python.h>
#include <structmember.h>

typedef enum P3118HelperQueue_QueueSlotType
{
    P3118HelperQueue_QueueSlotLinker_SLOT = 1,
    P3118HelperQueue_QueueSlotLinker_SENTINEL = 2
} P3118HelperQueue_QueueSlotType;

typedef struct P3118HelperQueue_QueueSlotLinker
{
    PyObject_HEAD
    struct P3118HelperQueue_QueueSlotLinker *prev;
    struct P3118HelperQueue_QueueSlotLinker *next;
    P3118HelperQueue_QueueSlotType type;
} P3118HelperQueue_QueueSlotLinker;

typedef struct P3118HelperQueue_QueueSentinelSlot_Object
{
    P3118HelperQueue_QueueSlotLinker linker;
} P3118HelperQueue_QueueSentinelSlot_Object;

typedef struct P3118HelperQueue_QueueStruct_Object P3118HelperQueue_QueueStruct_Object;
typedef struct P3118HelperQueue_QueueSlot_Object
{
    P3118HelperQueue_QueueSlotLinker linker;
    PyObject *uid;
    PyObject *display_name;
    P3118HelperQueue_QueueStruct_Object *master;
} P3118HelperQueue_QueueSlot_Object;


typedef struct P3118HelperQueue_QueueMeta
{
    P3118HelperQueue_QueueSentinelSlot_Object sentinel;
    P3118HelperQueue_QueueSlot_Object *first_slot;
    unsigned long long magic;
    P3118HelperQueue_QueueStruct_Object *master;
    PyObject *name;
} P3118HelperQueue_QueueMeta;

typedef struct P3118HelperQueue_QueueMeta_Object
{
    PyObject_HEAD
    P3118HelperQueue_QueueMeta
        body;
} P3118HelperQueue_QueueMeta_Object;

#define P3118HelperQueue_SlotsArena_MAX_CAPASITY (63)

typedef struct P3118HelperQueue_SlotsArena
{
    struct P3118HelperQueue_SlotsArena *prev;
    struct P3118HelperQueue_SlotsArena *next;
    /* char used[P3118HelperQueue_SlotsArena_MAX_CAPASITY]; */
    P3118HelperQueue_QueueSlot_Object slots[P3118HelperQueue_SlotsArena_MAX_CAPASITY];
} P3118HelperQueue_SlotsArena;

struct P3118HelperQueue_QueueStruct_Object
{
    PyObject_VAR_HEAD
    P3118HelperQueue_SlotsArena
        start_arena;
    unsigned long long magic;
    PyObject *name;
    P3118HelperQueue_QueueMeta_Object entries[1];
};

typedef struct P3118HelperQueue_QueueSlotForwardIterator
{
    PyObject_HEAD
    P3118HelperQueue_QueueSlotLinker *pos;
    unsigned long long magic;
    P3118HelperQueue_QueueMeta_Object *master;
} P3118HelperQueue_QueueSlotForwardIterator, P3118HelperQueue_QueueForwardIterator;

typedef struct P3118HelperQueue_QueueSlotReverseIterator
{
    PyObject_HEAD
    P3118HelperQueue_QueueSlotLinker *pos;
    unsigned long long magic;
    P3118HelperQueue_QueueMeta_Object *master;
} P3118HelperQueue_QueueSlotReverseIterator, P3118HelperQueue_QueueReverseIterator;


typedef struct P3118HelperQueue_QueueSlotInput_Object
{
    PyObject_HEAD
    PyObject *uid;
    PyObject *display_name;
} P3118HelperQueue_QueueSlotInput_Object;


static P3118HelperQueue_QueueSlotInput_Object *P3118HelperQueue_QueueSlotInput_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs);

static PyObject *P3118HelperQueue_QueueSlotInput_GetUID(P3118HelperQueue_QueueSlotInput_Object *self);

static int P3118HelperQueue_QueueSlotInput_SetUID(P3118HelperQueue_QueueSlotInput_Object *self, PyObject *value);

static PyObject *P3118HelperQueue_QueueSlotInput_GetDisplayName(P3118HelperQueue_QueueSlotInput_Object *self);

static int P3118HelperQueue_QueueSlotInput_SetDisplayName(P3118HelperQueue_QueueSlotInput_Object *self, PyObject *value);

static void P3118HelperQueue_QueueSlotInput_Dealloc(P3118HelperQueue_QueueSlotInput_Object *self);

static PyObject *P3118HelperQueue_QueueSlotInput_Str(P3118HelperQueue_QueueSlotInput_Object *self);

static PyObject *P3118HelperQueue_QueueSlotInput_Repr(P3118HelperQueue_QueueSlotInput_Object *self);

PyGetSetDef P3118HelperQueue_QueueSlotInput_GetSet[] = {
    {"uid",          (getter) P3118HelperQueue_QueueSlotInput_GetUID,         (setter) P3118HelperQueue_QueueSlotInput_SetUID},
    {"display_name", (getter) P3118HelperQueue_QueueSlotInput_GetDisplayName, (setter) P3118HelperQueue_QueueSlotInput_SetDisplayName},
    {NULL}
};

PyTypeObject P3118HelperQueue_QueueSlotInput_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.user",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueSlotInput_Object),
    .tp_new = (newfunc) P3118HelperQueue_QueueSlotInput_New,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueSlotInput_Dealloc,
    .tp_getset = P3118HelperQueue_QueueSlotInput_GetSet,
    .tp_str = (reprfunc) P3118HelperQueue_QueueSlotInput_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueSlotInput_Repr,
};

static P3118HelperQueue_QueueStruct_Object *P3118HelperQueue_QueueStruct_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs);


static PyObject *P3118HelperQueue_QueueStruct_Str(P3118HelperQueue_QueueStruct_Object *self);

static PyObject *P3118HelperQueue_QueueStruct_Repr(P3118HelperQueue_QueueStruct_Object *self);

static Py_ssize_t P3118HelperQueue_QueueStruct_Len(P3118HelperQueue_QueueStruct_Object *self);

static P3118HelperQueue_QueueMeta_Object *P3118HelperQueue_QueueStruct_GetItem(P3118HelperQueue_QueueStruct_Object *self, Py_ssize_t index);

static int P3118HelperQueue_QueueStruct_Traverse(P3118HelperQueue_QueueStruct_Object *self, visitproc visit, void *arg);

static int P3118HelperQueue_QueueStruct_Clear(P3118HelperQueue_QueueStruct_Object *self);

static void P3118HelperQueue_QueueStruct_Dealloc(P3118HelperQueue_QueueStruct_Object *self);

PySequenceMethods P3118HelperQueue_QueueStruct_Sequence = {
    .sq_length =(lenfunc) P3118HelperQueue_QueueStruct_Len,
    .sq_item = (ssizeargfunc) P3118HelperQueue_QueueStruct_GetItem
};

PyTypeObject P3118HelperQueue_QueueStruct_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueStruct_Object) - sizeof(P3118HelperQueue_QueueMeta_Object[1]),
    .tp_itemsize = sizeof(P3118HelperQueue_QueueMeta_Object),
    .tp_new = (newfunc) P3118HelperQueue_QueueStruct_New,
    .tp_str = (reprfunc) P3118HelperQueue_QueueStruct_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueStruct_Repr,
    .tp_as_sequence = &P3118HelperQueue_QueueStruct_Sequence,
    .tp_flags = Py_TPFLAGS_HAVE_GC,
    .tp_traverse = (traverseproc) P3118HelperQueue_QueueStruct_Traverse,
    .tp_clear = (inquiry) P3118HelperQueue_QueueStruct_Clear,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueStruct_Dealloc
};


static PyObject *P3118HelperQueue_QueueMeta_Str(P3118HelperQueue_QueueMeta_Object *self);

static PyObject *P3118HelperQueue_QueueMeta_Repr(P3118HelperQueue_QueueMeta_Object *self);


static Py_ssize_t P3118HelperQueue_QueueMeta_Len(P3118HelperQueue_QueueMeta_Object *self);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueMeta_GetItem(P3118HelperQueue_QueueMeta_Object *self, Py_ssize_t index);

static int P3118HelperQueue_QueueMeta_Traverse(P3118HelperQueue_QueueMeta_Object *self, visitproc visit, void *arg);

static int P3118HelperQueue_QueueMeta_Clear(P3118HelperQueue_QueueMeta_Object *self);

static void P3118HelperQueue_QueueMeta_Dealloc(P3118HelperQueue_QueueMeta_Object *self);


PyMappingMethods P3118HelperQueue_QueueMeta_Mapping = {
    .mp_length = (lenfunc) P3118HelperQueue_QueueMeta_Len, /* avoiding optimization of negative indices in .sq_item */
};

PySequenceMethods P3118HelperQueue_QueueMeta_Sequence = {
    .sq_item = (ssizeargfunc) P3118HelperQueue_QueueMeta_GetItem,
};


PyTypeObject P3118HelperQueue_QueueMeta_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.Queue",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueMeta_Object),
    .tp_str = (reprfunc) P3118HelperQueue_QueueMeta_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueMeta_Repr,
    .tp_as_mapping = &P3118HelperQueue_QueueMeta_Mapping,
    .tp_as_sequence = &P3118HelperQueue_QueueMeta_Sequence,
    .tp_flags = Py_TPFLAGS_HAVE_GC,
    .tp_traverse = (traverseproc) NULL,
    .tp_clear = (inquiry) P3118HelperQueue_QueueMeta_Clear,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueMeta_Dealloc,
};

static PyObject *P3118HelperQueue_QueueSlot_Str(P3118HelperQueue_QueueSlot_Object *self);

static PyObject *P3118HelperQueue_QueueSlot_Repr(P3118HelperQueue_QueueSlot_Object *self);

static int P3118HelperQueue_QueueSlot_Traverse(P3118HelperQueue_QueueSlot_Object *self, visitproc visit, void *arg);

static void P3118HelperQueue_QueueSlot_Dealloc(P3118HelperQueue_QueueSlot_Object *self);

PyTypeObject P3118HelperQueue_QueueSlot_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.QueueSlot",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueSlot_Object),
    .tp_str = (reprfunc) P3118HelperQueue_QueueSlot_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueSlot_Repr,
    .tp_flags = Py_TPFLAGS_HAVE_GC,
    .tp_traverse = (traverseproc) P3118HelperQueue_QueueSlot_Traverse,
    .tp_dealloc =(destructor) P3118HelperQueue_QueueSlot_Dealloc
};

static PyObject *P3118HelperQueue_QueueSentinelSlot_Repr(P3118HelperQueue_QueueSlot_Object *self);


static int P3118HelperQueue_QueueSentinelSlot_Traverse(P3118HelperQueue_QueueSentinelSlot_Object *self, visitproc visit, void *arg);

static void P3118HelperQueue_QueueSentinelSlot_Dealloc(P3118HelperQueue_QueueSentinelSlot_Object *self);

PyTypeObject P3118HelperQueue_QueueSentinelSlot_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.QueueSentinelSlot",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueSentinelSlot_Object),
    .tp_repr = (reprfunc) P3118HelperQueue_QueueSentinelSlot_Repr,
    .tp_flags = Py_TPFLAGS_HAVE_GC,
    .tp_traverse = (traverseproc) P3118HelperQueue_QueueSentinelSlot_Traverse,
    .tp_dealloc = P3118HelperQueue_QueueSentinelSlot_Dealloc
};

static P3118HelperQueue_QueueSlotInput_Object *P3118HelperQueue_QueueSlotInput_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"uid", "display_name", NULL};
    PyObject *uid;
    PyObject *display_name;
    P3118HelperQueue_QueueSlotInput_Object *self;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!U", kw_list, &PyLong_Type, &uid, &display_name))
    {
        return NULL;
    }

    self = (P3118HelperQueue_QueueSlotInput_Object *) cls->tp_alloc(cls, 0);
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    Py_INCREF(uid);
    self->uid = uid;
    Py_INCREF(display_name);
    self->display_name = display_name;
    return self;
}

static PyObject *P3118HelperQueue_QueueSlotInput_GetUID(P3118HelperQueue_QueueSlotInput_Object *self)
{
    Py_INCREF(self->uid);
    return self->uid;
}

static int P3118HelperQueue_QueueSlotInput_SetUID(P3118HelperQueue_QueueSlotInput_Object *self, PyObject *value)
{
    if (value == NULL)
    {
        PyErr_Format(PyExc_AttributeError, "can't delete UID");
        return -1;
    }
    if (!PyLong_CheckExact(value))
    {
        PyErr_Format(PyExc_AttributeError, "UID must be int (got %R)", Py_TYPE(value));
        return -1;
    }
    Py_DECREF(self->uid);
    Py_INCREF(value);
    self->uid = value;
    return 0;
}

static PyObject *P3118HelperQueue_QueueSlotInput_GetDisplayName(P3118HelperQueue_QueueSlotInput_Object *self)
{
    Py_INCREF(self->display_name);
    return self->display_name;
}

static int P3118HelperQueue_QueueSlotInput_SetDisplayName(P3118HelperQueue_QueueSlotInput_Object *self, PyObject *value)
{
    if (value == NULL)
    {
        PyErr_Format(PyExc_AttributeError, "can't delete display name");
        return -1;
    }
    if (!PyUnicode_CheckExact(value))
    {
        PyErr_Format(PyExc_AttributeError, "display name must be str (got %R)", Py_TYPE(value));
        return -1;
    }
    Py_DECREF(self->display_name);
    Py_INCREF(value);
    self->display_name = value;
    return 0;
}

static void P3118HelperQueue_QueueSlotInput_Dealloc(P3118HelperQueue_QueueSlotInput_Object *self)
{

    Py_DECREF(self->uid);
    Py_DECREF(self->display_name);
    Py_TYPE(self)->tp_free(self);
}

static PyObject *P3118HelperQueue_QueueSlotInput_Str(P3118HelperQueue_QueueSlotInput_Object *self)
{
    return PyUnicode_FromFormat(
        "<user for queue: uid=%S display_name=%A>",
        self->uid,
        self->display_name
    );
}

static PyObject *P3118HelperQueue_QueueSlotInput_Repr(P3118HelperQueue_QueueSlotInput_Object *self)
{
    return PyUnicode_FromFormat(
        "<%s object uid=%S display_name=%A at %p>",
        Py_TYPE(self)->tp_name,
        self->uid,
        self->display_name,
        self
    );
}

void P3118HelperQueue_SlotsArena_Init(P3118HelperQueue_SlotsArena *self)
{
    Py_ssize_t i;
    P3118HelperQueue_QueueSlot_Object initializer = {
        {
            PyObject_HEAD_INIT(NULL)
        }
    };

    self->prev = NULL;
    self->next = NULL;
    for (i = 0; i < P3118HelperQueue_SlotsArena_MAX_CAPASITY; i++)
    {
        self->slots[i] = initializer;
    }
}

P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_SlotsArena_Malloc(P3118HelperQueue_SlotsArena *arena)
{
    Py_ssize_t i;
    while (1)
    {
        for (i = 0; i < P3118HelperQueue_SlotsArena_MAX_CAPASITY; i++)
        {
            if (Py_TYPE(&(arena->slots[i])) == NULL)
            {
                Py_TYPE(&(arena->slots[i])) = &P3118HelperQueue_QueueSlot_Type;
                Py_REFCNT(&(arena->slots[i])) = 1;
                arena->slots[i].linker.prev = NULL;
                arena->slots[i].linker.next = NULL;
                arena->slots[i].linker.type = P3118HelperQueue_QueueSlotLinker_SLOT;
                arena->slots[i].uid = NULL;
                arena->slots[i].display_name = NULL;
                arena->slots[i].master = NULL;
                return &(arena->slots[i]);
            }
        }
        if (arena->next == NULL)
        {
            arena->next = PyMem_Malloc(sizeof(P3118HelperQueue_SlotsArena));
            if (arena->next == NULL)
            {
                PyErr_NoMemory();
                return NULL;
            }
            P3118HelperQueue_SlotsArena_Init(arena->next);
        }
        arena = arena->next;
    }
}

void P3118HelperQueue_SlotsArena_Free(P3118HelperQueue_SlotsArena *arena, P3118HelperQueue_QueueSlot_Object *slot)
{
    while (arena != NULL)
    {
        if ((uintptr_t) &(arena->slots[0]) <= (uintptr_t) slot && (uintptr_t) slot < (uintptr_t) &(arena->slots[P3118HelperQueue_SlotsArena_MAX_CAPASITY]))
        {
            Py_TYPE(slot) = NULL;
            return;
        }
        arena = arena->next;
    }
    Py_FatalError("Can't deallocate queue slot");
}

void P3118HelperQueue_QueueStruct_Init(P3118HelperQueue_QueueStruct_Object *self)
{
    Py_ssize_t i;
    P3118HelperQueue_QueueMeta_Object initializer = {
        PyObject_HEAD_INIT(&P3118HelperQueue_QueueMeta_Type)
        {
            .sentinel = {
                {
                    PyObject_HEAD_INIT(&P3118HelperQueue_QueueSentinelSlot_Type /* todo really need this class? */)
                    .prev = NULL,
                    .next = NULL,
                    .type = P3118HelperQueue_QueueSlotLinker_SENTINEL
                }
            },
            .first_slot = NULL,
            .magic = 0,
            .master = self,
            .name = NULL
        }
    };

    P3118HelperQueue_SlotsArena_Init(&(self->start_arena));

    self->magic = 0;
    self->name = NULL;
    for (i = Py_SIZE(self) - 1; i >= 0; i--)
    {
        Py_INCREF(self);
        self->entries[i] = initializer;
        self->entries[i].body.first_slot = (P3118HelperQueue_QueueSlot_Object *) &(self->entries[i].body.sentinel);
        /* Py_INCREF(self->entries[i].body.first_slot); */
    }
}

static P3118HelperQueue_QueueStruct_Object *P3118HelperQueue_QueueStruct_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"name", "queues", NULL};
    PyObject *name;
    PyObject *queues;
    P3118HelperQueue_QueueStruct_Object *self;
    PyObject *current_subqueue;
    PyObject *current_subqueue_iterator;
    PyObject *queue_name;
    Py_ssize_t dict_iter_pos = 0;
    P3118HelperQueue_QueueMeta_Object *current_queue_meta;
    P3118HelperQueue_QueueSlot_Object *last_slot = NULL;
    P3118HelperQueue_QueueSlot_Object *current_slot;
    PyObject *last_item;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UO", kw_list, &name, &queues))
    {
        return NULL;
    }

    if (Py_TYPE(queues) == &PyDictProxy_Type || PyDict_Check(queues))
    {
        if (PyDict_GET_SIZE(queues) < 1)
        {
            PyErr_Format(PyExc_TypeError, "empty queues are not supported");
            return NULL;
        }
        self = (P3118HelperQueue_QueueStruct_Object *) cls->tp_alloc(cls, PyDict_GET_SIZE(queues));
        if (self == NULL)
        {
            PyErr_NoMemory();
            return NULL;
        }
        P3118HelperQueue_QueueStruct_Init(self);
        Py_INCREF(name);
        self->name = name;

        current_queue_meta = &(self->entries[0]);

        while (PyDict_Next(queues, &dict_iter_pos, &queue_name, &current_subqueue))
        {
#if PY_VERSION_HEX < 0x03060000
            PyErr_Format(
                PyExc_TypeError,
                "for python version (current %d.%d.%d) lower then 3.6.* constructing %s from dict is not available because dicts are not ordered",
                PY_MAJOR_VERSION,
                PY_MINOR_VERSION,
                PY_MICRO_VERSION,
                P3118HelperQueue_QueueStruct_Type.tp_name
            );
            Py_DECREF(self);
            return NULL;
#endif
            if (!PyUnicode_CheckExact(queue_name))
            {
                Py_DECREF(self);
                PyErr_Format(PyExc_TypeError, "queue name must be str");
                return NULL;
            }
            Py_INCREF(queue_name);
            current_queue_meta->body.name = queue_name;

            proc_queue:
            current_subqueue_iterator = PyObject_GetIter(current_subqueue);
            if (current_subqueue_iterator == NULL)
            {
                Py_DECREF(self);
                return NULL;
            }

            while ((last_item = PyIter_Next(current_subqueue_iterator)) != NULL)
            {
                current_slot = P3118HelperQueue_SlotsArena_Malloc(&(self->start_arena));
                if (current_slot == NULL)
                {
                    Py_DECREF(last_item);
                    Py_DECREF(current_subqueue_iterator);
                    Py_DECREF(self);
                    return NULL;
                }

                if (last_slot != NULL)
                {
                    /* Py_DECREF(last_slot); /* not need because reference from next slot already obtained by sentinel */
                    Py_INCREF(current_slot);
                    last_slot->linker.next = (P3118HelperQueue_QueueSlotLinker *) current_slot;
                    current_slot->linker.prev = (P3118HelperQueue_QueueSlotLinker *) last_slot;
                }
                Py_INCREF(current_slot);
                current_slot->linker.next = (P3118HelperQueue_QueueSlotLinker *) &(current_queue_meta->body.sentinel);
                current_queue_meta->body.sentinel.linker.prev = (P3118HelperQueue_QueueSlotLinker *) current_slot;
                if (current_queue_meta->body.first_slot == (P3118HelperQueue_QueueSlot_Object *) &(current_queue_meta->body.sentinel))
                {
                    Py_INCREF(current_slot);
                    current_queue_meta->body.first_slot = current_slot;
                }

                if (Py_TYPE(last_item) == &P3118HelperQueue_QueueSlotInput_Type)
                {
                    current_slot->uid = ((P3118HelperQueue_QueueSlotInput_Object *) last_item)->uid;
                    Py_INCREF(current_slot->uid);
                    current_slot->display_name = ((P3118HelperQueue_QueueSlotInput_Object *) last_item)->display_name;
                    Py_INCREF(current_slot->display_name);
                }
                    /*
                    else if (PyTuple_CheckExact(last_item))
                    {

                    }
                    */
                else if (last_item == Py_None)
                {
                    current_slot->uid = NULL;
                    current_slot->display_name = NULL;
                }
                else
                {
                    PyErr_Format(
                        PyExc_TypeError,
                        "user data must be '%s', tuple or None (got %R)",
                        P3118HelperQueue_QueueSlotInput_Type.tp_name,
                        Py_TYPE(last_item)
                    );
                    Py_DECREF(last_item);
                    Py_DECREF(current_subqueue_iterator);
                    Py_DECREF(self);
                    return NULL;
                }
                Py_DECREF(last_item);
                last_slot = current_slot;
            }

            if (dict_iter_pos == 0) /* true if PyDict_Next not called <=> entered by goto from single queue */
            {
                break;
            }
            last_slot = (P3118HelperQueue_QueueSlot_Object *) &(current_queue_meta->body.sentinel);
            current_queue_meta++;
        }
    }
    else
    {
        self = (P3118HelperQueue_QueueStruct_Object *) cls->tp_alloc(cls, 1);
        if (self == NULL)
        {
            PyErr_NoMemory();
            return NULL;
        }
        P3118HelperQueue_QueueStruct_Init(self);
        Py_INCREF(name);
        self->name = name;

        current_queue_meta = &(self->entries[0]);

        current_subqueue = queues;
        queue_name = NULL;
        goto proc_queue;
    }

    printf("# %d\n", Py_REFCNT(&(self->entries[0])));
    return self;
}

static PyObject *P3118HelperQueue_QueueStruct_Str(P3118HelperQueue_QueueStruct_Object *self)
{
    return PyUnicode_FromFormat(
        "<%s queue struct: name=%A size=%d>",
        Py_TYPE(self)->tp_name,
        self->name,
        Py_SIZE(self)
    );
}

static PyObject *P3118HelperQueue_QueueStruct_Repr(P3118HelperQueue_QueueStruct_Object *self)
{
    return PyUnicode_FromFormat(
        "<%s queue struct object name=%A size=%d at %p>",
        Py_TYPE(self)->tp_name,
        self->name,
        Py_SIZE(self),
        self
    );
}


static Py_ssize_t P3118HelperQueue_QueueStruct_Len(P3118HelperQueue_QueueStruct_Object *self)
{
    return Py_SIZE(self);
}

static P3118HelperQueue_QueueMeta_Object *P3118HelperQueue_QueueStruct_GetItem(P3118HelperQueue_QueueStruct_Object *self, Py_ssize_t index)
{
    if (index < 0)
    {
        index += Py_SIZE(self);
    }
    if (index < 0 || index >= Py_SIZE(self))
    {
        PyErr_Format(PyExc_IndexError, "queue struct index out of range");
        return NULL;
    }

    Py_INCREF(&self->entries[index]);
    return &self->entries[index];
}

static int P3118HelperQueue_QueueStruct_Traverse(P3118HelperQueue_QueueStruct_Object *self, visitproc visit, void *arg)
{
    Py_ssize_t i;
    printf("qs tr 0x%p %d\n", self, Py_REFCNT(self));
    for (i = Py_SIZE(self) - 1; i >= 0; i--)
    {
        printf("i %d %s %d\n", i, Py_TYPE(&(self->entries[i]))->tp_name, Py_REFCNT(&(self->entries[i])));
        Py_VISIT(&(self->entries[i]));
        printf("di %d\n", Py_REFCNT(&(self->entries[i])));
    }
    return 0;
}

static int P3118HelperQueue_QueueStruct_Clear(P3118HelperQueue_QueueStruct_Object *self)
{
    Py_ssize_t i;

    printf("qs cl\n");
    for (i = Py_SIZE(self) - 1; i >= 0; i--)
    {
        Py_DECREF(&(self->entries[i]));
    }
    return 0;
}

static void P3118HelperQueue_QueueStruct_Dealloc(P3118HelperQueue_QueueStruct_Object *self)
{
    P3118HelperQueue_SlotsArena *removed_arena;

    printf("qs del\n");
    while ((removed_arena = self->start_arena.next) != NULL)
    {
        self->start_arena.next = removed_arena->next;
        PyMem_Free(removed_arena);
    }

    Py_TYPE(self)->tp_free(self);
}


static PyObject *P3118HelperQueue_QueueMeta_Str(P3118HelperQueue_QueueMeta_Object *self)
{
    if (self->body.name == NULL)
    {
        return PyUnicode_FromFormat("<queue>");
    }
    else
    {
        return PyUnicode_FromFormat("<queue: name=%A>", self->body.name);
    }
}

static PyObject *P3118HelperQueue_QueueMeta_Repr(P3118HelperQueue_QueueMeta_Object *self)
{
    if (self->body.name == NULL)
    {
        return PyUnicode_FromFormat("<%s queue object at %p>", Py_TYPE(self)->tp_name, self);
    }
    else
    {
        return PyUnicode_FromFormat("<%s queue object name=%A at %p>", Py_TYPE(self)->tp_name, self->body.name, self);
    }
}

static void P3118HelperQueue_QueueMeta_Dealloc(P3118HelperQueue_QueueMeta_Object *self)
{
    printf("qm del\n");
    Py_DECREF(self->body.master);
    Py_DECREF(self->body.name);
}

static Py_ssize_t P3118HelperQueue_QueueMeta_Len(P3118HelperQueue_QueueMeta_Object *self)
{
    P3118HelperQueue_QueueSlotLinker *p;
    Py_ssize_t L = 0;
    for (p = (P3118HelperQueue_QueueSlotLinker *) (self->body.first_slot); p != NULL && p->type != P3118HelperQueue_QueueSlotLinker_SENTINEL; p = p->next)
    {
        L++;
    }
    return L;
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueMeta_GetItem(P3118HelperQueue_QueueMeta_Object *self, Py_ssize_t index)
{
    P3118HelperQueue_QueueSlotLinker *p;
    if (index >= 0)
    {
        p = (P3118HelperQueue_QueueSlotLinker *) (self->body.first_slot);
        while (p != NULL && p->type != P3118HelperQueue_QueueSlotLinker_SENTINEL && index > 0)
        {
            p = p->next;
            index--;
        }
    }
    else
    {
        p = (P3118HelperQueue_QueueSlotLinker *) (self->body.sentinel.linker.prev);
        index++;
        while (p != NULL && p->type != P3118HelperQueue_QueueSlotLinker_SENTINEL && index < 0)
        {
            p = p->prev;
            index++;
        }
    }
    if (index != 0 || p == NULL || p->type == P3118HelperQueue_QueueSlotLinker_SENTINEL)
    {
        PyErr_Format(PyExc_TypeError, "queue index out of range");
        return NULL;
    }
    Py_INCREF(p);
    return (P3118HelperQueue_QueueSlot_Object *) p;
}

static int P3118HelperQueue_QueueMeta_Traverse(P3118HelperQueue_QueueMeta_Object *self, visitproc visit, void *arg)
{
    printf("qm tr\n");
    Py_VISIT(self->body.master);
    Py_VISIT(self->body.first_slot);
    return 0;
}


static int P3118HelperQueue_QueueMeta_PopLast(P3118HelperQueue_QueueMeta *self)
{
    P3118HelperQueue_QueueSlot_Object *removed_slot;
    if (self->first_slot == (P3118HelperQueue_QueueSlot_Object *) &(self->sentinel))
    {
        return 0;
    }

    removed_slot = (P3118HelperQueue_QueueSlot_Object *) self->sentinel.linker.prev;
    if (self->first_slot == removed_slot)
    {
        Py_DECREF(removed_slot);
        self->first_slot = (P3118HelperQueue_QueueSlot_Object *) &(self->sentinel);
    }
    removed_slot->linker.prev->next = removed_slot->linker.next;
    removed_slot->linker.next->prev = removed_slot->linker.prev;
    removed_slot->linker.prev = NULL;
    removed_slot->linker.next = NULL;
    removed_slot->master = self->master;
    Py_INCREF(removed_slot->master);
    Py_DECREF(removed_slot);
    Py_DECREF(removed_slot);
    self->magic++;
    self->master->magic++;
    return 1;
}

static int P3118HelperQueue_QueueMeta_Clear(P3118HelperQueue_QueueMeta_Object *self)
{
    while (P3118HelperQueue_QueueMeta_PopLast(&(self->body)))
    {}
    return 0;
}

static PyObject *P3118HelperQueue_QueueSlot_Str(P3118HelperQueue_QueueSlot_Object *self)
{
    if (self->uid == NULL)
    {
        return PyUnicode_FromFormat("<empty queue slot>");
    }
    else
    {
        return PyUnicode_FromFormat("<queue slot: uid=%d display_name=%A>", self->uid, self->display_name);
    }
}

static PyObject *P3118HelperQueue_QueueSlot_Repr(P3118HelperQueue_QueueSlot_Object *self)
{
    if (self->uid == NULL)
    {
        return PyUnicode_FromFormat("<%s queue slot object at %p>", Py_TYPE(self)->tp_name, self);
    }
    else
    {
        return PyUnicode_FromFormat("<%s queue slot object uid=%d display_name=%A at %p>", Py_TYPE(self)->tp_name, self->uid, self->display_name, self);
    }
}

static int P3118HelperQueue_QueueSlot_Traverse(P3118HelperQueue_QueueSlot_Object *self, visitproc visit, void *arg)
{
    if (self->linker.next != NULL)
    {
        Py_VISIT(self->linker.next);
    }
    if (self->linker.prev != NULL)
    {
        Py_VISIT(self->linker.prev);
    }
    if (self->master != NULL)
    {
        Py_VISIT(self->master);
    }
    return 0;
}

static void P3118HelperQueue_QueueSlot_Dealloc(P3118HelperQueue_QueueSlot_Object *self)
{
    P3118HelperQueue_QueueStruct_Object *master;
    Py_XDECREF(self->uid);
    Py_XDECREF(self->display_name);

    printf("qsl del\n");

    if (self->linker.prev != NULL)
    {
        self->linker.prev->next = self->linker.next;
    }
    else
    {
        Py_XDECREF(self->linker.next);
    }
    if (self->linker.prev != NULL)
    {
        self->linker.next->prev = self->linker.prev;
    }
    else
    {
        Py_XDECREF(self->linker.prev);
    }

    master = self->master;
    if (master == NULL)
    {
        Py_FatalError("Trying dealloc bound queue slot");
        return;
    }
    P3118HelperQueue_SlotsArena_Free(&(master->start_arena), self);
    Py_DECREF(master);
}


static PyObject *P3118HelperQueue_QueueSentinelSlot_Repr(P3118HelperQueue_QueueSlot_Object *self)
{
    return PyUnicode_FromFormat("<queue sentinel slot at %p>", self);
}

static int P3118HelperQueue_QueueSentinelSlot_Traverse(P3118HelperQueue_QueueSentinelSlot_Object *self, visitproc visit, void *arg)
{
    if (self->linker.prev != NULL)
    {
        Py_VISIT(self->linker.prev);
    }
    if (self->linker.next != NULL)
    {
        Py_VISIT(self->linker.next);
    }
    return 0;
}

static void P3118HelperQueue_QueueSentinelSlot_Dealloc(P3118HelperQueue_QueueSentinelSlot_Object *self)
{}

PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    .m_name = "p3118helper.queue"
};

PyMODINIT_FUNC PyInit__queue(void)
{
    PyObject *module;
    PyObject *qs_dict;

    if (PyType_Ready(&P3118HelperQueue_QueueSlotInput_Type))
    {
        return NULL;
    }
    if (PyType_Ready(&P3118HelperQueue_QueueMeta_Type))
    {
        return NULL;
    }
    if (PyType_Ready(&P3118HelperQueue_QueueSlot_Type))
    {
        return NULL;
    }

    if (PyType_Ready(&P3118HelperQueue_QueueSentinelSlot_Type))
    {
        return NULL;
    }

    qs_dict = PyDict_New();
    if (qs_dict == NULL)
    {
        return NULL;
    }

    if (PyDict_SetItemString(qs_dict, "user", (PyObject *) &P3118HelperQueue_QueueSlotInput_Type) != 0)
    {
        Py_DECREF(qs_dict);
        return NULL;
    }
    if (PyDict_SetItemString(qs_dict, "Queue", (PyObject *) &P3118HelperQueue_QueueMeta_Type) != 0)
    {
        Py_DECREF(qs_dict);
        return NULL;
    }
    if (PyDict_SetItemString(qs_dict, "QueueSlot", (PyObject *) &P3118HelperQueue_QueueSlot_Type) != 0)
    {
        Py_DECREF(qs_dict);
        return NULL;
    }

    P3118HelperQueue_QueueStruct_Type.tp_dict = qs_dict;
    if (PyType_Ready(&P3118HelperQueue_QueueStruct_Type))
    {
        return NULL;
    }


    module = PyModule_Create(&module_def);
    if (module == NULL)
    {
        return NULL;
    }

    if (PyModule_AddObject(module, "QueueStruct", (PyObject *) &P3118HelperQueue_QueueStruct_Type) != 0)
    {
        Py_DECREF(module);
        return NULL;
    }

    return module;


}