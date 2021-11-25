
#include <math.h>
#include <float.h>

#include "msg_compare.h"

#define EPSILON 0.000001

static bool double_equals(double a, double b)
{
    return (fabs(a - b) < (EPSILON * fabs(a + b)));
}

static bool double_gt(float a, float b)
{
    return (a - b) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * EPSILON);
}

static bool double_lt(float a, float b)
{
    return (b - a) > ( (fabs(a) < fabs(b) ? fabs(b) : fabs(a)) * EPSILON);
}

static int compare_doubles(double event, double message, enum cmp_op comparison)
{
    bool is_eq = double_equals(event, message);
    bool is_gt = double_gt(event, message);
    bool is_lt = double_lt(event, message);
    switch (comparison) {
    case EQ:
        if (is_eq) return SUCCESS;
        break;
    case NEQ:
        if (!is_eq) return SUCCESS;
        break;
    case GT:
        if (is_gt) return SUCCESS;
        break;
    case GTEQ:
        if (is_gt || is_eq) return SUCCESS;
        break;
    case LT:
        if (is_lt) return SUCCESS;
        break;
    case LTEQ:
        if (is_lt || is_eq) return SUCCESS;
        break;
    default:
        return UNSUPPORTED;
    }
    return NOT_EQUAL;
}

static int compare_string(const char* s1, const char* s2, enum cmp_op comparison)
{
    int ans = strcmp(s1, s2);
    switch (comparison) {
    case EQ:
        if (ans == 0) return SUCCESS;
        break;
    case NEQ:
        if (ans != 0) return SUCCESS;
        break;
    default:
        return UNSUPPORTED;
    }
    return NOT_EQUAL;
}

int compare_messages(struct message *event, struct message *received, enum cmp_op comparison)
{
    if (strcmp(event->value_type, received->value_type) != 0) {
        return NOT_EQUAL;
    }

    if (strcmp(event->value_type, MSG_NUMBER_TYPE) == 0) {
        double event_num = strtod(event->value, NULL);
        double received_num = strtod(received->value, NULL);
        return compare_doubles(event_num, received_num, comparison);
    }
    else if (strcmp(event->value_type, MSG_STRING_TYPE) == 0) {
        return compare_string(event->value, received->value, comparison);
    }
    return PARAM_ERR;
}
