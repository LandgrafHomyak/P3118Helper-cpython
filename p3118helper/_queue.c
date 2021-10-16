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

typedef struct P3118HelperQueue_QueueStruct_Object P3118HelperQueue_QueueStruct_Object;
typedef struct P3118HelperQueue_QueueMeta_Object P3118HelperQueue_QueueMeta_Object;

typedef struct P3118HelperQueue_QueueSlot_Object
{
    P3118HelperQueue_QueueSlotLinker linker;
    PyObject *uid;
    PyObject *display_name;
    P3118HelperQueue_QueueMeta_Object *master;
} P3118HelperQueue_QueueSlot_Object;

struct P3118HelperQueue_QueueMeta_Object
{
    P3118HelperQueue_QueueSlotLinker sentinel;
    P3118HelperQueue_QueueSlot_Object *first_slot;
    unsigned long long magic;
    P3118HelperQueue_QueueStruct_Object *master;
    PyObject *name;
};

#define P3118HelperQueue_SlotsArena_MAX_CAPASITY (63)

typedef struct P3118HelperQueue_SlotsArena
{
    struct P3118HelperQueue_SlotsArena *next;
    P3118HelperQueue_QueueSlot_Object slots[P3118HelperQueue_SlotsArena_MAX_CAPASITY];
} P3118HelperQueue_SlotsArena;

struct P3118HelperQueue_QueueStruct_Object
{
    PyObject_VAR_HEAD
    P3118HelperQueue_SlotsArena start_arena;
    unsigned long long magic;
    PyObject *name;
    char final;
    P3118HelperQueue_QueueMeta_Object entries[1];
};

typedef struct P3118HelperQueue_QueueForwardIterator_Object
{
    PyObject_HEAD
    P3118HelperQueue_QueueSlotLinker *pos;
    unsigned long long magic;
    P3118HelperQueue_QueueMeta_Object *master;
} P3118HelperQueue_QueueForwardIterator_Object;

typedef struct P3118HelperQueue_QueueReverseIterator_Object
{
    PyObject_HEAD
    P3118HelperQueue_QueueSlotLinker *pos;
    unsigned long long magic;
    P3118HelperQueue_QueueMeta_Object *master;
} P3118HelperQueue_QueueReverseIterator_Object;

typedef struct P3118HelperQueue_QueueStructSlotIterator_Object
{
    PyObject_HEAD
    P3118HelperQueue_QueueSlotLinker *pos;
    unsigned long long magic;
    P3118HelperQueue_QueueStruct_Object *master;
} P3118HelperQueue_QueueStructSlotIterator_Object;

typedef struct P3118HelperQueue_QueueSlotInput_Object
{
    PyObject_HEAD
    PyObject *uid;
    PyObject *display_name;
} P3118HelperQueue_QueueSlotInput_Object;

typedef struct P3118HelperQueue_MessageKey_Object
{
    PyObject_HEAD
    PyObject *chat;
    PyObject *message;
    Py_hash_t hash;
} P3118HelperQueue_MessageKey_Object;


static P3118HelperQueue_QueueSlotInput_Object *P3118HelperQueue_QueueSlotInput_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs);

static PyObject *P3118HelperQueue_QueueSlotInput_GetUID(P3118HelperQueue_QueueSlotInput_Object *self);

static int P3118HelperQueue_QueueSlotInput_SetUID(P3118HelperQueue_QueueSlotInput_Object *self, PyObject *value);

static PyObject *P3118HelperQueue_QueueSlotInput_GetDisplayName(P3118HelperQueue_QueueSlotInput_Object *self);

static int P3118HelperQueue_QueueSlotInput_SetDisplayName(P3118HelperQueue_QueueSlotInput_Object *self, PyObject *value);

static void P3118HelperQueue_QueueSlotInput_Dealloc(P3118HelperQueue_QueueSlotInput_Object *self);

static PyObject *P3118HelperQueue_QueueSlotInput_Str(P3118HelperQueue_QueueSlotInput_Object *self);

static PyObject *P3118HelperQueue_QueueSlotInput_Repr(P3118HelperQueue_QueueSlotInput_Object *self);

static PyGetSetDef P3118HelperQueue_QueueSlotInput_GetSet[] = {
    {"uid",          (getter) P3118HelperQueue_QueueSlotInput_GetUID,         (setter) P3118HelperQueue_QueueSlotInput_SetUID},
    {"display_name", (getter) P3118HelperQueue_QueueSlotInput_GetDisplayName, (setter) P3118HelperQueue_QueueSlotInput_SetDisplayName},
    {NULL}
};

static PyTypeObject P3118HelperQueue_QueueSlotInput_Type = {
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

static void P3118HelperQueue_QueueStruct_Dealloc(P3118HelperQueue_QueueStruct_Object *self);

static P3118HelperQueue_QueueStructSlotIterator_Object *P3118HelperQueue_QueueStruct_IterSlots(P3118HelperQueue_QueueStruct_Object *self);

static PyObject *P3118HelperQueue_QueueStruct_FindUser(P3118HelperQueue_QueueStruct_Object *self, PyObject *uid);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueStruct_Push(P3118HelperQueue_QueueStruct_Object *self, PyObject *args);

static PyObject *P3118HelperQueue_QueueStruct_Finalize(P3118HelperQueue_QueueStruct_Object *self);

static PySequenceMethods P3118HelperQueue_QueueStruct_Sequence = {
    .sq_length =(lenfunc) P3118HelperQueue_QueueStruct_Len,
    .sq_item = (ssizeargfunc) P3118HelperQueue_QueueStruct_GetItem
};

static PyMethodDef P3118HelperQueue_QueueStruct_Methods[] = {
    {"iter_slots", (PyCFunction) P3118HelperQueue_QueueStruct_IterSlots, METH_NOARGS},
    {"find_user",  (PyCFunction) P3118HelperQueue_QueueStruct_FindUser,  METH_O},
    {"push",       (PyCFunction) P3118HelperQueue_QueueStruct_Push,      METH_VARARGS},
    {"finalize",       (PyCFunction) P3118HelperQueue_QueueStruct_Finalize,      METH_NOARGS},
    {NULL}
};

static PyMemberDef P3118HelperQueue_QueueStruct_Members[] = {
    {"name",  T_OBJECT_EX, offsetof(P3118HelperQueue_QueueStruct_Object, name),  READONLY},
    {"final", T_BOOL,      offsetof(P3118HelperQueue_QueueStruct_Object, final), READONLY},
    {NULL}
};

static PyTypeObject P3118HelperQueue_QueueStruct_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueStruct_Object) - sizeof(P3118HelperQueue_QueueMeta_Object[1]),
    .tp_itemsize = sizeof(P3118HelperQueue_QueueMeta_Object),
    .tp_new = (newfunc) P3118HelperQueue_QueueStruct_New,
    .tp_str = (reprfunc) P3118HelperQueue_QueueStruct_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueStruct_Repr,
    .tp_as_sequence = &P3118HelperQueue_QueueStruct_Sequence,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueStruct_Dealloc,
    .tp_flags = Py_TPFLAGS_BASETYPE,
    .tp_methods = P3118HelperQueue_QueueStruct_Methods,
    .tp_members = P3118HelperQueue_QueueStruct_Members,
};


static PyObject *P3118HelperQueue_QueueMeta_Str(P3118HelperQueue_QueueMeta_Object *self);

static PyObject *P3118HelperQueue_QueueMeta_Repr(P3118HelperQueue_QueueMeta_Object *self);

static Py_ssize_t P3118HelperQueue_QueueMeta_Len(P3118HelperQueue_QueueMeta_Object *self);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueMeta_GetItem(P3118HelperQueue_QueueMeta_Object *self, Py_ssize_t index);

static void P3118HelperQueue_QueueMeta_Dealloc(P3118HelperQueue_QueueMeta_Object *self);

static P3118HelperQueue_QueueForwardIterator_Object *P3118HelperQueue_QueueMeta_ForwardIter(P3118HelperQueue_QueueMeta_Object *self);

static P3118HelperQueue_QueueReverseIterator_Object *P3118HelperQueue_QueueMeta_ReverseIter(P3118HelperQueue_QueueMeta_Object *self);

static PyMappingMethods P3118HelperQueue_QueueMeta_Mapping = {
    .mp_length = (lenfunc) P3118HelperQueue_QueueMeta_Len, /* avoiding optimization of negative indices in .sq_item */
};

static PySequenceMethods P3118HelperQueue_QueueMeta_Sequence = {
    .sq_item = (ssizeargfunc) P3118HelperQueue_QueueMeta_GetItem,
};

static PyMethodDef P3118HelperQueue_QueueMeta_Methods[] = {
    {"__reversed__", (PyCFunction) P3118HelperQueue_QueueMeta_ReverseIter, METH_NOARGS},
    {NULL}
};

static PyMemberDef P3118HelperQueue_QueueMeta_Members[] = {
    {"name", T_OBJECT, offsetof(P3118HelperQueue_QueueMeta_Object, name), READONLY},
    {NULL}
};

static PyTypeObject P3118HelperQueue_QueueMeta_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.Queue",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueMeta_Object),
    .tp_str = (reprfunc) P3118HelperQueue_QueueMeta_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueMeta_Repr,
    .tp_as_mapping = &P3118HelperQueue_QueueMeta_Mapping,
    .tp_as_sequence = &P3118HelperQueue_QueueMeta_Sequence,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueMeta_Dealloc,
    .tp_iter = (getiterfunc) P3118HelperQueue_QueueMeta_ForwardIter,
    .tp_methods = P3118HelperQueue_QueueMeta_Methods,
    .tp_members = P3118HelperQueue_QueueMeta_Members
};


static PyObject *P3118HelperQueue_QueueSlot_Str(P3118HelperQueue_QueueSlot_Object *self);

static PyObject *P3118HelperQueue_QueueSlot_Repr(P3118HelperQueue_QueueSlot_Object *self);

static void P3118HelperQueue_QueueSlot_Dealloc(P3118HelperQueue_QueueSlot_Object *self);

static PyObject *P3118HelperQueue_QueueSlot_GetDisplayName(P3118HelperQueue_QueueSlot_Object *self);

static int P3118HelperQueue_QueueSlot_SetDisplayName(P3118HelperQueue_QueueSlot_Object *self, PyObject *value);

static int P3118HelperQueue_QueueSlot_Bool(P3118HelperQueue_QueueSlot_Object *self);

static PyObject *P3118HelperQueue_QueueSlot_Clear(P3118HelperQueue_QueueSlot_Object *self);

static PyObject *P3118HelperQueue_QueueSlot_Fill(P3118HelperQueue_QueueSlot_Object *self, P3118HelperQueue_QueueSlotInput_Object *data);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueSlot_Up(P3118HelperQueue_QueueSlot_Object *self);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueSlot_Down(P3118HelperQueue_QueueSlot_Object *self);

static PyMemberDef P3118HelperQueue_QueueSlot_Members[] = {
    {"uid", T_OBJECT, offsetof(P3118HelperQueue_QueueSlot_Object, uid), READONLY},
    {NULL}
};

static PyGetSetDef P3118HelperQueue_QueueSlot_GetSet[] = {
    {"display_name", (getter) P3118HelperQueue_QueueSlot_GetDisplayName, (setter) P3118HelperQueue_QueueSlot_SetDisplayName},
    {NULL}
};

static PyNumberMethods P3118HelperQueue_QueueSlot_Number = {
    .nb_bool =(inquiry) P3118HelperQueue_QueueSlot_Bool
};

static PyMethodDef P3118HelperQueue_QueueSlot_Methods[] = {
    {"clear", (PyCFunction) P3118HelperQueue_QueueSlot_Clear, METH_NOARGS},
    {"fill",  (PyCFunction) P3118HelperQueue_QueueSlot_Fill,  METH_O},
    {"up",    (PyCFunction) P3118HelperQueue_QueueSlot_Up,    METH_NOARGS},
    {"down",  (PyCFunction) P3118HelperQueue_QueueSlot_Down,  METH_NOARGS},
    {NULL}
};

static PyTypeObject P3118HelperQueue_QueueSlot_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.QueueSlot",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueSlot_Object),
    .tp_str = (reprfunc) P3118HelperQueue_QueueSlot_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueSlot_Repr,
    .tp_dealloc =(destructor) P3118HelperQueue_QueueSlot_Dealloc,
    .tp_members = P3118HelperQueue_QueueSlot_Members,
    .tp_getset = P3118HelperQueue_QueueSlot_GetSet,
    .tp_as_number = &P3118HelperQueue_QueueSlot_Number,
    .tp_methods = P3118HelperQueue_QueueSlot_Methods
};


static PyObject *iter_self(PyObject *self);

static PyObject *P3118HelperQueue_QueueForwardIterator_Str(P3118HelperQueue_QueueForwardIterator_Object *self);

static PyObject *P3118HelperQueue_QueueForwardIterator_Repr(P3118HelperQueue_QueueForwardIterator_Object *self);

static void P3118HelperQueue_QueueForwardIterator_Dealloc(P3118HelperQueue_QueueForwardIterator_Object *self);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueForwardIterator_Next(P3118HelperQueue_QueueForwardIterator_Object *self);

static PyTypeObject P3118HelperQueue_QueueForwardIterator_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.Queue.forward_iterator",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueForwardIterator_Object),
    .tp_str = (reprfunc) P3118HelperQueue_QueueForwardIterator_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueForwardIterator_Repr,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueForwardIterator_Dealloc,
    .tp_iter = (getiterfunc) iter_self,
    .tp_iternext = (iternextfunc) P3118HelperQueue_QueueForwardIterator_Next
};


static PyObject *P3118HelperQueue_QueueReverseIterator_Str(P3118HelperQueue_QueueReverseIterator_Object *self);

static PyObject *P3118HelperQueue_QueueReverseIterator_Repr(P3118HelperQueue_QueueReverseIterator_Object *self);

static void P3118HelperQueue_QueueReverseIterator_Dealloc(P3118HelperQueue_QueueReverseIterator_Object *self);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueReverseIterator_Next(P3118HelperQueue_QueueReverseIterator_Object *self);

static PyTypeObject P3118HelperQueue_QueueReverseIterator_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.Queue.reverse_iterator",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueReverseIterator_Object),
    .tp_str = (reprfunc) P3118HelperQueue_QueueReverseIterator_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueReverseIterator_Repr,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueReverseIterator_Dealloc,
    .tp_iter = (getiterfunc) iter_self,
    .tp_iternext = (iternextfunc) P3118HelperQueue_QueueReverseIterator_Next
};

static PyObject *P3118HelperQueue_QueueStructSlotIterator_Str(P3118HelperQueue_QueueStructSlotIterator_Object *self);

static PyObject *P3118HelperQueue_QueueStructSlotIterator_Repr(P3118HelperQueue_QueueStructSlotIterator_Object *self);

static void P3118HelperQueue_QueueStructSlotIterator_Dealloc(P3118HelperQueue_QueueStructSlotIterator_Object *self);

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueStructSlotIterator_Next(P3118HelperQueue_QueueStructSlotIterator_Object *self);

static PyTypeObject P3118HelperQueue_QueueStructSlotIterator_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueStruct.slot_iterator",
    .tp_basicsize = sizeof(P3118HelperQueue_QueueStructSlotIterator_Object),
    .tp_str = (reprfunc) P3118HelperQueue_QueueStructSlotIterator_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_QueueStructSlotIterator_Repr,
    .tp_dealloc = (destructor) P3118HelperQueue_QueueStructSlotIterator_Dealloc,
    .tp_iter = (getiterfunc) iter_self,
    .tp_iternext = (iternextfunc) P3118HelperQueue_QueueStructSlotIterator_Next
};

static P3118HelperQueue_MessageKey_Object *P3118HelperQueue_MessageKey_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs);

static P3118HelperQueue_MessageKey_Object *P3118HelperQueue_MessageKey_FromMessage(PyTypeObject *cls, PyObject *message);

static PyObject *P3118HelperQueue_MessageKey_Str(P3118HelperQueue_MessageKey_Object *self);

static PyObject *P3118HelperQueue_MessageKey_Repr(P3118HelperQueue_MessageKey_Object *self);

static void P3118HelperQueue_MessageKey_Dealloc(P3118HelperQueue_MessageKey_Object *self);

static Py_hash_t P3118HelperQueue_MessageKey_Hash(P3118HelperQueue_MessageKey_Object *self);

static PyObject *P3118HelperQueue_MessageKey_Compare(P3118HelperQueue_MessageKey_Object *self, P3118HelperQueue_MessageKey_Object *other, int op);

static PyMethodDef P3118HelperQueue_MessageKey_Methods[] = {
    {"from_message", (PyCFunction) P3118HelperQueue_MessageKey_FromMessage, METH_CLASS | METH_O},
    {NULL}
};

static PyMemberDef P3118HelperQueue_MessageKey_Members[] = {
    {"chat",    T_OBJECT_EX, offsetof(P3118HelperQueue_MessageKey_Object, chat),    READONLY},
    {"message", T_OBJECT_EX, offsetof(P3118HelperQueue_MessageKey_Object, message), READONLY},
    {NULL}
};

static PyTypeObject P3118HelperQueue_MessageKey_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "p3118helper.queue.QueueMessage.message_key",
    .tp_basicsize = sizeof(P3118HelperQueue_MessageKey_Object),
    .tp_new = (newfunc) P3118HelperQueue_MessageKey_New,
    .tp_str = (reprfunc) P3118HelperQueue_MessageKey_Str,
    .tp_repr = (reprfunc) P3118HelperQueue_MessageKey_Repr,
    .tp_dealloc = (destructor) P3118HelperQueue_MessageKey_Dealloc,
    .tp_hash = (hashfunc) P3118HelperQueue_MessageKey_Hash,
    .tp_richcompare = (richcmpfunc) P3118HelperQueue_MessageKey_Compare,
    .tp_methods = P3118HelperQueue_MessageKey_Methods,
    .tp_members = P3118HelperQueue_MessageKey_Members
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

static void P3118HelperQueue_SlotsArena_Init(P3118HelperQueue_SlotsArena *self)
{
    Py_ssize_t i;
    P3118HelperQueue_QueueSlot_Object initializer = {
        {
            PyObject_HEAD_INIT(NULL)
        }
    };

    self->next = NULL;
    for (i = 0; i < P3118HelperQueue_SlotsArena_MAX_CAPASITY; i++)
    {
        self->slots[i] = initializer;
    }
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_SlotsArena_Alloc(P3118HelperQueue_SlotsArena *arena)
{
    Py_ssize_t i;
    while (1)
    {
        for (i = 0; i < P3118HelperQueue_SlotsArena_MAX_CAPASITY; i++)
        {
            if (Py_TYPE(&(arena->slots[i])) == NULL)
            {
                Py_TYPE(&(arena->slots[i])) = &P3118HelperQueue_QueueSlot_Type;
                Py_REFCNT(&(arena->slots[i])) = 0;
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

static void P3118HelperQueue_SlotsArena_Free(P3118HelperQueue_SlotsArena *arena, P3118HelperQueue_QueueSlot_Object *slot)
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

static void P3118HelperQueue_QueueStruct_Init(P3118HelperQueue_QueueStruct_Object *self)
{
    Py_ssize_t i;
    P3118HelperQueue_QueueMeta_Object initializer = {
        .sentinel = {
            PyObject_HEAD_INIT(&P3118HelperQueue_QueueMeta_Type)
            .prev = NULL,
            .next = NULL,
            .type = P3118HelperQueue_QueueSlotLinker_SENTINEL
        },
        .first_slot = NULL,
        .magic = 0,
        .master = self,
        .name = NULL
    };
    Py_REFCNT(&initializer) = 0;

    P3118HelperQueue_SlotsArena_Init(&(self->start_arena));

    self->magic = 0;
    self->name = NULL;
    for (i = Py_SIZE(self) - 1; i >= 0; i--)
    {
        self->entries[i] = initializer;
        self->entries[i].first_slot = (P3118HelperQueue_QueueSlot_Object *) &(self->entries[i]);
    }

}

static P3118HelperQueue_QueueMeta_Object *P3118HelperQueue_QueueMeta_IncRef(P3118HelperQueue_QueueMeta_Object *self)
{
    if (Py_REFCNT(self) == 0)
    {
        Py_INCREF(self->master);
    }
    Py_INCREF(self);
    return self;
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueSlot_IncRef(P3118HelperQueue_QueueSlot_Object *self)
{
    if (Py_REFCNT(self) == 0)
    {
        P3118HelperQueue_QueueMeta_IncRef(self->master);
    }
    Py_INCREF(self);
    return self;
}

static P3118HelperQueue_QueueStruct_Object *P3118HelperQueue_QueueStruct_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"name", "final", "queues", NULL};
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
    int final;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "UpO", kw_list, &name, &final, &queues))
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
            current_queue_meta->name = queue_name;

            proc_queue:
            current_subqueue_iterator = PyObject_GetIter(current_subqueue);
            if (current_subqueue_iterator == NULL)
            {
                Py_DECREF(self);
                return NULL;
            }

            while ((last_item = PyIter_Next(current_subqueue_iterator)) != NULL)
            {
                current_slot = P3118HelperQueue_SlotsArena_Alloc(&(self->start_arena));
                if (current_slot == NULL)
                {
                    Py_DECREF(last_item);
                    Py_DECREF(current_subqueue_iterator);
                    Py_DECREF(self);
                    return NULL;
                }

                if (last_slot != NULL)
                {
                    last_slot->linker.next = (P3118HelperQueue_QueueSlotLinker *) current_slot;
                    current_slot->linker.prev = (P3118HelperQueue_QueueSlotLinker *) last_slot;
                }
                current_slot->linker.next = (P3118HelperQueue_QueueSlotLinker *) current_queue_meta;
                current_queue_meta->sentinel.prev = (P3118HelperQueue_QueueSlotLinker *) current_slot;
                if (current_queue_meta->first_slot == (P3118HelperQueue_QueueSlot_Object *) current_queue_meta)
                {
                    current_queue_meta->first_slot = current_slot;
                }
                current_slot->master = current_queue_meta;

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
            last_slot = (P3118HelperQueue_QueueSlot_Object *) current_queue_meta;
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
    self->final = (char) final;
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

    return P3118HelperQueue_QueueMeta_IncRef(&(self->entries[index]));
}


static int P3118HelperQueue_QueueMeta_PopLast(P3118HelperQueue_QueueMeta_Object *self)
{
    P3118HelperQueue_QueueSlot_Object *removed_slot;

    if (self->first_slot == (P3118HelperQueue_QueueSlot_Object *) self || self->sentinel.prev == NULL)
    {
        return 0;
    }
    removed_slot = (P3118HelperQueue_QueueSlot_Object *) self->sentinel.prev;
    if (self->first_slot == removed_slot)
    {
        self->first_slot = (P3118HelperQueue_QueueSlot_Object *) removed_slot->linker.next;
    }
    if (removed_slot->linker.prev != NULL)
    {
        removed_slot->linker.prev->next = removed_slot->linker.next;
    }
    if (removed_slot->linker.next != NULL)
    {
        removed_slot->linker.next->prev = removed_slot->linker.prev;
    }
    removed_slot->linker.prev = NULL;
    removed_slot->linker.next = NULL;
    removed_slot->master = self;
    self->magic++;
    self->master->magic++;
    if (Py_REFCNT(self->master) > 0)
    {
        P3118HelperQueue_QueueSlot_IncRef(removed_slot);
        if (Py_REFCNT(removed_slot) == 1)
        {
            Py_DECREF(removed_slot); /* triggering dealloc */
        }
    }
    return 1;
}

static void P3118HelperQueue_QueueMeta_RemoveExtra(P3118HelperQueue_QueueMeta_Object *self)
{
    while (self->sentinel.prev != NULL && self->sentinel.prev->type != P3118HelperQueue_QueueSlotLinker_SENTINEL && ((P3118HelperQueue_QueueSlot_Object *) (self->sentinel.prev))->uid == NULL)
    {
        P3118HelperQueue_QueueMeta_PopLast(self);
    }
}

static void P3118HelperQueue_QueueStruct_Dealloc(P3118HelperQueue_QueueStruct_Object *self)
{
    P3118HelperQueue_SlotsArena *removed_arena;
    Py_ssize_t i;
    Py_DECREF(self->name);
    for (i = Py_SIZE(self) - 1; i >= 0; i--)
    {
        Py_XDECREF(self->entries[i].name);
        while (P3118HelperQueue_QueueMeta_PopLast(&(self->entries[i])))
        {}
    }

    while ((removed_arena = self->start_arena.next) != NULL)
    {
        self->start_arena.next = removed_arena->next;
        PyMem_Free(removed_arena);
    }

    Py_TYPE(self)->tp_free(self);
}

static P3118HelperQueue_QueueStructSlotIterator_Object *P3118HelperQueue_QueueStruct_IterSlots(P3118HelperQueue_QueueStruct_Object *self)
{
    P3118HelperQueue_QueueStructSlotIterator_Object *iterator;

    iterator = (P3118HelperQueue_QueueStructSlotIterator_Object *) P3118HelperQueue_QueueStructSlotIterator_Type.tp_alloc(&P3118HelperQueue_QueueStructSlotIterator_Type, 0);
    if (iterator == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    iterator->magic = self->magic;
    iterator->master = self;
    Py_INCREF(self);
    iterator->pos = (P3118HelperQueue_QueueSlotLinker *) self->entries[0].first_slot;
    while (iterator->pos != NULL && iterator->pos->type == P3118HelperQueue_QueueSlotLinker_SENTINEL)
    {
        iterator->pos = iterator->pos->next;
    }

    return iterator;
}

static PyObject *P3118HelperQueue_QueueStruct_FindUser(P3118HelperQueue_QueueStruct_Object *self, PyObject *uid)
{
    P3118HelperQueue_QueueSlot_Object *p;

    if (!PyLong_CheckExact(uid))
    {
        PyErr_Format(PyExc_TypeError, "uid must be int (got %R)", Py_TYPE(uid));
        return NULL;
    }


    for (p = self->entries[0].first_slot; p != NULL; p = (P3118HelperQueue_QueueSlot_Object *) p->linker.next)
    {
        if (p->linker.type != P3118HelperQueue_QueueSlotLinker_SENTINEL)
        {
            if (p->uid != NULL)
            {
                switch (PyObject_RichCompareBool(p->uid, uid, Py_EQ))
                {
                    case 1:
                        return (PyObject *) P3118HelperQueue_QueueSlot_IncRef(p);
                    case 0:
                        break;
                    case -1:
                        return NULL;
                }
            }
        }
    }
    Py_RETURN_NONE;
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueStruct_Push(P3118HelperQueue_QueueStruct_Object *self, PyObject *args)
{
    P3118HelperQueue_QueueSlotInput_Object *data;
    P3118HelperQueue_QueueSlot_Object *slot;
    Py_ssize_t i = 0;

    if (!PyArg_ParseTuple(args, "O!|n", &P3118HelperQueue_QueueSlotInput_Type, &data, &i))
    {
        return NULL;
    }

    if (i < 0)
    {
        i += Py_SIZE(self);
    }

    if (i < 0 || i >= Py_SIZE(self))
    {
        PyErr_Format(PyExc_IndexError, "queue struct index out of range");
        return NULL;
    }

    slot = (P3118HelperQueue_QueueSlot_Object *) P3118HelperQueue_QueueStruct_FindUser(self, data->uid);
    if (slot == NULL)
    {
        return NULL;
    }
    if ((PyObject *) slot != Py_None)
    {
        Py_DECREF(slot);
        PyErr_Format(PyExc_KeyError, "user with id %S already in this queue", data->uid);
        return NULL;
    }
    Py_DECREF(slot);

    slot = self->entries[i].first_slot;
    while (slot != NULL && slot != (P3118HelperQueue_QueueSlot_Object *) &(self->entries[i]) && slot->uid != NULL)
    {
        slot = (P3118HelperQueue_QueueSlot_Object *) slot->linker.next;
    }


    if (slot == NULL || slot == (P3118HelperQueue_QueueSlot_Object *) &(self->entries[i]))
    {
        slot = P3118HelperQueue_SlotsArena_Alloc(&(self->start_arena));
        if (slot == NULL)
        {
            return NULL;
        }
        slot->linker.prev = self->entries[i].sentinel.prev;
        if (slot->linker.prev != NULL)
        {
            slot->linker.prev->next = (P3118HelperQueue_QueueSlotLinker *) slot;
        }
        slot->linker.next = (P3118HelperQueue_QueueSlotLinker *) &(self->entries[i]);
        self->entries[i].sentinel.prev = (P3118HelperQueue_QueueSlotLinker *) slot;
        if (self->entries[i].first_slot == (P3118HelperQueue_QueueSlot_Object *) &(self->entries[i]))
        {
            self->entries[i].first_slot = slot;
        }
        slot->master = &(self->entries[i]);
        self->magic++;
        self->entries[i].magic++;
    }
    Py_INCREF(data->uid);
    slot->uid = data->uid;
    Py_INCREF(data->display_name);
    slot->display_name = data->display_name;

    return P3118HelperQueue_QueueSlot_IncRef(slot);
}

static PyObject *P3118HelperQueue_QueueStruct_Finalize(P3118HelperQueue_QueueStruct_Object *self)
{
    self->final = 1;
    Py_RETURN_NONE;
}

static PyObject *P3118HelperQueue_QueueMeta_Str(P3118HelperQueue_QueueMeta_Object *self)
{
    if (self->name == NULL)
    {
        return PyUnicode_FromFormat("<queue>");
    }
    else
    {
        return PyUnicode_FromFormat("<queue: name=%A>", self->name);
    }
}

static PyObject *P3118HelperQueue_QueueMeta_Repr(P3118HelperQueue_QueueMeta_Object *self)
{
    if (self->name == NULL)
    {
        return PyUnicode_FromFormat("<%s queue object at %p>", Py_TYPE(self)->tp_name, self);
    }
    else
    {
        return PyUnicode_FromFormat("<%s queue object name=%A at %p>", Py_TYPE(self)->tp_name, self->name, self);
    }
}

static void P3118HelperQueue_QueueMeta_Dealloc(P3118HelperQueue_QueueMeta_Object *self)
{
    Py_DECREF(self->master);
}

static P3118HelperQueue_QueueForwardIterator_Object *P3118HelperQueue_QueueMeta_ForwardIter(P3118HelperQueue_QueueMeta_Object *self)
{
    P3118HelperQueue_QueueForwardIterator_Object *iterator;

    iterator = (P3118HelperQueue_QueueForwardIterator_Object *) P3118HelperQueue_QueueForwardIterator_Type.tp_alloc(&P3118HelperQueue_QueueForwardIterator_Type, 0);
    if (iterator == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    if (self->first_slot == (P3118HelperQueue_QueueSlot_Object *) self)
    {
        iterator->pos = NULL;
    }
    else
    {
        iterator->pos = (P3118HelperQueue_QueueSlotLinker *) self->first_slot;
    }
    iterator->magic = self->magic;
    iterator->master = P3118HelperQueue_QueueMeta_IncRef(self);

    return iterator;
}

static P3118HelperQueue_QueueReverseIterator_Object *P3118HelperQueue_QueueMeta_ReverseIter(P3118HelperQueue_QueueMeta_Object *self)
{
    P3118HelperQueue_QueueReverseIterator_Object *iterator;

    iterator = (P3118HelperQueue_QueueReverseIterator_Object *) P3118HelperQueue_QueueReverseIterator_Type.tp_alloc(&P3118HelperQueue_QueueReverseIterator_Type, 0);
    if (iterator == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }

    if (self->first_slot == (P3118HelperQueue_QueueSlot_Object *) self)
    {
        iterator->pos = NULL;
    }
    else
    {
        iterator->pos = self->sentinel.prev;
    }
    iterator->magic = self->magic;
    iterator->master = P3118HelperQueue_QueueMeta_IncRef(self);

    return iterator;
}


static Py_ssize_t P3118HelperQueue_QueueMeta_Len(P3118HelperQueue_QueueMeta_Object *self)
{
    P3118HelperQueue_QueueSlotLinker *p;
    Py_ssize_t L = 0;
    for (p = (P3118HelperQueue_QueueSlotLinker *) (self->first_slot); p != NULL && p != (P3118HelperQueue_QueueSlotLinker *) self; p = p->next)
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
        p = (P3118HelperQueue_QueueSlotLinker *) (self->first_slot);
        while (p != NULL && p != (P3118HelperQueue_QueueSlotLinker *) self && index > 0)
        {
            p = p->next;
            index--;
        }
    }
    else
    {
        p = (P3118HelperQueue_QueueSlotLinker *) (self->sentinel.prev);
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
    return P3118HelperQueue_QueueSlot_IncRef((P3118HelperQueue_QueueSlot_Object *) p);
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

static void P3118HelperQueue_QueueSlot_Dealloc(P3118HelperQueue_QueueSlot_Object *self)
{
    P3118HelperQueue_QueueMeta_Object *master;

    if (self->linker.next != NULL || self->linker.prev != NULL)
    {
        Py_DECREF(self->master);
        return;
    }

    Py_XDECREF(self->uid);
    Py_XDECREF(self->display_name);

    master = self->master;
    P3118HelperQueue_SlotsArena_Free(&(master->master->start_arena), self);
    Py_DECREF(master);
}

static PyObject *P3118HelperQueue_QueueSlot_GetDisplayName(P3118HelperQueue_QueueSlot_Object *self)
{
    if (self->display_name == NULL)
    {
        Py_RETURN_NONE;
    }

    Py_INCREF(self->display_name);
    return self->display_name;
}

static int P3118HelperQueue_QueueSlot_SetDisplayName(P3118HelperQueue_QueueSlot_Object *self, PyObject *value)
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
    if (self->uid == NULL)
    {
        PyErr_Format(PyExc_TypeError, "this slot is empty, use .fill(...)");
        return -1;
    }
    Py_DECREF(self->display_name);
    Py_INCREF(value);
    self->display_name = value;
    return 0;
}

static int P3118HelperQueue_QueueSlot_Bool(P3118HelperQueue_QueueSlot_Object *self)
{
    return self->uid != NULL;
}

static PyObject *P3118HelperQueue_QueueSlot_Clear(P3118HelperQueue_QueueSlot_Object *self)
{
    Py_XDECREF(self->uid);
    self->uid = NULL;
    Py_XDECREF(self->display_name);
    self->display_name = NULL;

    P3118HelperQueue_QueueMeta_RemoveExtra(self->master);

    Py_RETURN_NONE;
}

static PyObject *P3118HelperQueue_QueueSlot_Fill(P3118HelperQueue_QueueSlot_Object *self, P3118HelperQueue_QueueSlotInput_Object *data)
{
    if (self->uid != NULL)
    {
        PyErr_Format(PyExc_ValueError, "slot is not empty");
        return NULL;
    }
/* todo */
    if (Py_TYPE(data) != &P3118HelperQueue_QueueSlotInput_Type)
    {

        PyErr_Format(PyExc_TypeError, "data must be '%s' (got %R)", P3118HelperQueue_QueueSlotInput_Type.tp_name, Py_TYPE(data));
        return NULL;
    }

    Py_INCREF(data->uid);
    self->uid = data->uid;
    Py_INCREF(data->display_name);
    self->display_name = data->display_name;

    Py_RETURN_NONE;
}


static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueSlot_Up(P3118HelperQueue_QueueSlot_Object *self)
{
    P3118HelperQueue_QueueSlot_Object *p;

    p = self;
    while (p != NULL && p->linker.type != P3118HelperQueue_QueueSlotLinker_SENTINEL && p->uid != NULL)
    {
        p = (P3118HelperQueue_QueueSlot_Object *) p->linker.prev;
    }

    if (p == NULL || p->linker.type == P3118HelperQueue_QueueSlotLinker_SENTINEL)
    {
        return P3118HelperQueue_QueueSlot_IncRef(self);
    }

    p->uid = self->uid;
    p->display_name = self->display_name;
    self->uid = NULL;
    self->display_name = NULL;

    P3118HelperQueue_QueueMeta_RemoveExtra(self->master);

    return P3118HelperQueue_QueueSlot_IncRef(p);
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueSlot_Down(P3118HelperQueue_QueueSlot_Object *self)
{
    P3118HelperQueue_QueueSlot_Object *p;

    p = self;
    while (p != NULL && p != (P3118HelperQueue_QueueSlot_Object *) self->master && p->uid != NULL)
    {
        p = (P3118HelperQueue_QueueSlot_Object *) p->linker.next;
    }

    if (p == NULL || p == (P3118HelperQueue_QueueSlot_Object *) self->master)
    { /* todo add append */
        p = P3118HelperQueue_SlotsArena_Alloc(&(self->master->master->start_arena));
        if (p == NULL)
        {
            return NULL;
        }
        p->linker.prev = self->master->sentinel.prev;
        if (p->linker.prev != NULL)
        {
            p->linker.prev->next = (P3118HelperQueue_QueueSlotLinker *) p;
        }
        p->linker.next = (P3118HelperQueue_QueueSlotLinker *) self->master;
        self->master->sentinel.prev = (P3118HelperQueue_QueueSlotLinker *) p;
        if (self->master->first_slot == (P3118HelperQueue_QueueSlot_Object *) self->master)
        {
            self->master->first_slot = p;
        }
        p->master = self->master;
        p->master->magic++;
        p->master->master->magic++;
    }


    p->uid = self->uid;
    p->display_name = self->display_name;
    self->uid = NULL;
    self->display_name = NULL;

    return P3118HelperQueue_QueueSlot_IncRef(p);
}

static PyObject *iter_self(PyObject *self)
{
    Py_INCREF(self);
    return self;
}

static PyObject *P3118HelperQueue_QueueForwardIterator_Str(P3118HelperQueue_QueueForwardIterator_Object *self)
{
    if (self->pos != NULL)
    {
        return PyUnicode_FromFormat("<queue forward iterator of %S @ %S>", self->master, self->pos);
    }
    else
    {
        return PyUnicode_FromFormat("<queue forward iterator of %S (ended)>", self->master);
    }
}

static PyObject *P3118HelperQueue_QueueForwardIterator_Repr(P3118HelperQueue_QueueForwardIterator_Object *self)
{
    if (self->pos != NULL)
    {
        return PyUnicode_FromFormat("<queue forward iterator object of %R @ %R at %p>", self->master, self->pos, self);
    }
    else
    {
        return PyUnicode_FromFormat("<queue forward iterator object of %R (ended) at %p>", self->master, self);
    }
}

static void P3118HelperQueue_QueueForwardIterator_Dealloc(P3118HelperQueue_QueueForwardIterator_Object *self)
{
    Py_DECREF(self->master);
    Py_TYPE(self)->tp_free(self);
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueForwardIterator_Next(P3118HelperQueue_QueueForwardIterator_Object *self)
{
    P3118HelperQueue_QueueSlotLinker *ret = self->pos;

    if (self->magic != self->master->magic)
    {
        self->pos = NULL;
        PyErr_Format(PyExc_RuntimeError, "queue was changed during iteration");
        return NULL;
    }

    if (ret == NULL)
    {
        return NULL;
    }

    self->pos = self->pos->next;
    if (self->pos == (P3118HelperQueue_QueueSlotLinker *) self->master)
    {
        self->pos = NULL;
    }

    return P3118HelperQueue_QueueSlot_IncRef((P3118HelperQueue_QueueSlot_Object *) ret);
}


static PyObject *P3118HelperQueue_QueueReverseIterator_Str(P3118HelperQueue_QueueReverseIterator_Object *self)
{
    if (self->pos != NULL)
    {
        return PyUnicode_FromFormat("<queue reverse iterator of %S @ %S>", self->master, self->pos);
    }
    else
    {
        return PyUnicode_FromFormat("<queue reverse iterator of %S (ended)>", self->master);
    }
}

static PyObject *P3118HelperQueue_QueueReverseIterator_Repr(P3118HelperQueue_QueueReverseIterator_Object *self)
{
    if (self->pos != NULL)
    {
        return PyUnicode_FromFormat("<queue reverse iterator object of %R @ %R at %p>", self->master, self->pos, self);
    }
    else
    {
        return PyUnicode_FromFormat("<queue reverse iterator object of %R (ended) at %p>", self->master, self);
    }
}

static void P3118HelperQueue_QueueReverseIterator_Dealloc(P3118HelperQueue_QueueReverseIterator_Object *self)
{
    Py_DECREF(self->master);
    Py_TYPE(self)->tp_free(self);
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueReverseIterator_Next(P3118HelperQueue_QueueReverseIterator_Object *self)
{
    P3118HelperQueue_QueueSlotLinker *ret = self->pos;

    if (self->magic != self->master->magic)
    {
        self->pos = NULL;
        PyErr_Format(PyExc_RuntimeError, "queue was changed during iteration");
        return NULL;
    }

    if (ret == NULL)
    {
        return NULL;
    }

    self->pos = self->pos->prev;
    if (self->pos != NULL && self->pos->type == P3118HelperQueue_QueueSlotLinker_SENTINEL)
    {
        self->pos = NULL;
    }

    return P3118HelperQueue_QueueSlot_IncRef((P3118HelperQueue_QueueSlot_Object *) ret);
}

static PyObject *P3118HelperQueue_QueueStructSlotIterator_Str(P3118HelperQueue_QueueStructSlotIterator_Object *self)
{
    if (self->pos != NULL)
    {
        return PyUnicode_FromFormat("<queue struct slot iterator of %S @ %S>", self->master, self->pos);
    }
    else
    {
        return PyUnicode_FromFormat("<queue struct slot iterator of %S (ended)>", self->master);
    }
}

static PyObject *P3118HelperQueue_QueueStructSlotIterator_Repr(P3118HelperQueue_QueueStructSlotIterator_Object *self)
{
    if (self->pos != NULL)
    {
        return PyUnicode_FromFormat("<queue struct slot iterator object of %R @ %R at %p>", self->master, self->pos, self);
    }
    else
    {
        return PyUnicode_FromFormat("<queue struct slot iterator object of %R (ended) at %p>", self->master, self);
    }
}

static void P3118HelperQueue_QueueStructSlotIterator_Dealloc(P3118HelperQueue_QueueStructSlotIterator_Object *self)
{
    Py_DECREF(self->master);
    Py_TYPE(self)->tp_free(self);
}

static P3118HelperQueue_QueueSlot_Object *P3118HelperQueue_QueueStructSlotIterator_Next(P3118HelperQueue_QueueStructSlotIterator_Object *self)
{
    P3118HelperQueue_QueueSlotLinker *ret = self->pos;

    if (self->magic != self->master->magic)
    {
        self->pos = NULL;
        PyErr_Format(PyExc_RuntimeError, "queue struct was changed during iteration");
        return NULL;
    }

    if (ret == NULL)
    {
        return NULL;
    }

    self->pos = self->pos->next;
    while (self->pos != NULL && self->pos->type == P3118HelperQueue_QueueSlotLinker_SENTINEL)
    {
        self->pos = self->pos->next;
    }

    return P3118HelperQueue_QueueSlot_IncRef((P3118HelperQueue_QueueSlot_Object *) ret);
}

static P3118HelperQueue_MessageKey_Object *P3118HelperQueue_MessageKey_Create(PyTypeObject *cls, PyObject *chat, PyObject *message)
{
    PyObject *tuple;
    Py_hash_t hash;
    P3118HelperQueue_MessageKey_Object *self;

    tuple = PyTuple_Pack(2, chat, message);
    if (tuple == NULL)
    {
        return NULL;
    }

    hash = PyObject_Hash(tuple);
    Py_DECREF(tuple);
    if (hash == -1 && PyErr_Occurred())
    {
        return NULL;
    }

    self = (P3118HelperQueue_MessageKey_Object *) cls->tp_alloc(cls, 0);
    if (self == NULL)
    {
        PyErr_NoMemory();
        return NULL;
    }
    Py_INCREF(chat);
    self->chat = chat;
    Py_INCREF(message);
    self->message = message;
    self->hash = hash;

    return self;
}


static P3118HelperQueue_MessageKey_Object *P3118HelperQueue_MessageKey_New(PyTypeObject *cls, PyObject *args, PyObject *kwargs)
{
    static char *kw_list[] = {"chat", "message", NULL};
    PyObject *chat;
    PyObject *message;

    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O!O!", kw_list, &PyLong_Type, &chat, &PyLong_Type, &message))
    {
        return NULL;
    }

    return P3118HelperQueue_MessageKey_Create(cls, chat, message);
}

static P3118HelperQueue_MessageKey_Object *P3118HelperQueue_MessageKey_FromMessage(PyTypeObject *cls, PyObject *message_object)
{
    PyObject *chat_object;
    PyObject *chat;
    PyObject *message;
    P3118HelperQueue_MessageKey_Object *self;

    message = PyObject_GetAttrString(message_object, "message_id");
    if (message == NULL)
    {
        return NULL;
    }
    if (!PyLong_CheckExact(message))
    {
        PyErr_Format(PyExc_TypeError, ".message_id must be int (got %R)", Py_TYPE(message));
        Py_DECREF(message);
        return NULL;
    }
    chat_object = PyObject_GetAttrString(message_object, "chat");
    if (chat_object == NULL)
    {
        Py_DECREF(message);
        return NULL;
    }

    chat = PyObject_GetAttrString(chat_object, "id");
    Py_DECREF(chat_object);
    if (chat == NULL)
    {
        Py_DECREF(message);
        return NULL;
    }

    if (!PyLong_CheckExact(chat))
    {
        PyErr_Format(PyExc_TypeError, ".chat.id must be int (got %R)", Py_TYPE(message));
        Py_DECREF(message);
        Py_DECREF(chat);
        return NULL;
    }

    self = P3118HelperQueue_MessageKey_Create(cls, chat, message);
    Py_DECREF(message);
    Py_DECREF(chat);
    if (self == NULL)
    {
        return NULL;
    }
    return self;
}


static PyObject *P3118HelperQueue_MessageKey_Str(P3118HelperQueue_MessageKey_Object *self)
{
    return PyUnicode_FromFormat("<message key c=%S m=%S>", self->chat, self->message);
}

static PyObject *P3118HelperQueue_MessageKey_Repr(P3118HelperQueue_MessageKey_Object *self)
{
    return PyUnicode_FromFormat("<message key object c=%S m=%S at %p>", self->chat, self->message, self);
}

static void P3118HelperQueue_MessageKey_Dealloc(P3118HelperQueue_MessageKey_Object *self)
{
    Py_DECREF(self->chat);
    Py_DECREF(self->message);
    Py_TYPE(self)->tp_free(self);
}

static Py_hash_t P3118HelperQueue_MessageKey_Hash(P3118HelperQueue_MessageKey_Object *self)
{
    return self->hash;
}

static PyObject *P3118HelperQueue_MessageKey_Compare(P3118HelperQueue_MessageKey_Object *self, P3118HelperQueue_MessageKey_Object *other, int op)
{
    if (op != Py_EQ)
    {
        Py_RETURN_NOTIMPLEMENTED;
    }

    if (Py_TYPE(other) != Py_TYPE(self))
    {
        PyErr_Format(PyExc_TypeError, "can't compare %R and %R", Py_TYPE(self), Py_TYPE(other));
        return NULL;
    }

    switch (PyObject_RichCompareBool(self->chat, other->chat, Py_EQ))
    {
        case 1:
            break;
        case 0:
            Py_RETURN_FALSE;
        case -1:
            return NULL;
    }

    switch (PyObject_RichCompareBool(self->message, other->message, Py_EQ))
    {
        case 1:
            break;
        case 0:
            Py_RETURN_FALSE;
        case -1:
            return NULL;
    }

    Py_RETURN_TRUE;
}

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    .m_name = "p3118helper.queue"
};

PyMODINIT_FUNC PyInit__queue(void)
{
    PyObject *module;
    PyObject *qs_dict;
    PyObject *qm_dict;

    if (PyType_Ready(&P3118HelperQueue_QueueSlotInput_Type))
    {
        return NULL;
    }
    if (PyType_Ready(&P3118HelperQueue_QueueForwardIterator_Type))
    {
        return NULL;
    }
    if (PyType_Ready(&P3118HelperQueue_QueueReverseIterator_Type))
    {
        return NULL;
    }


    qm_dict = PyDict_New();
    if (qm_dict == NULL)
    {
        return NULL;
    }

    if (PyDict_SetItemString(qm_dict, "forward_iterator", (PyObject *) &P3118HelperQueue_QueueForwardIterator_Type) != 0)
    {
        Py_DECREF(qm_dict);
        return NULL;
    }
    if (PyDict_SetItemString(qm_dict, "reverse_iterator", (PyObject *) &P3118HelperQueue_QueueReverseIterator_Type) != 0)
    {
        Py_DECREF(qm_dict);
        return NULL;
    }

    P3118HelperQueue_QueueMeta_Type.tp_dict = qm_dict;

    if (PyType_Ready(&P3118HelperQueue_QueueMeta_Type))
    {
        return NULL;
    }
    if (PyType_Ready(&P3118HelperQueue_QueueSlot_Type))
    {
        return NULL;
    }
    if (PyType_Ready(&P3118HelperQueue_QueueStructSlotIterator_Type))
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
    if (PyDict_SetItemString(qs_dict, "slot_iterator", (PyObject *) &P3118HelperQueue_QueueStructSlotIterator_Type) != 0)
    {
        Py_DECREF(qs_dict);
        return NULL;
    }

    P3118HelperQueue_QueueStruct_Type.tp_dict = qs_dict;
    if (PyType_Ready(&P3118HelperQueue_QueueStruct_Type))
    {
        return NULL;
    }

    if (PyType_Ready(&P3118HelperQueue_MessageKey_Type))
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

    if (PyModule_AddObject(module, "message_key", (PyObject *) &P3118HelperQueue_MessageKey_Type) != 0)
    {
        Py_DECREF(module);
        return NULL;
    }

    return module;
}