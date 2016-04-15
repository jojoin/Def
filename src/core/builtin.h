#pragma once



// �ڽ����Ժ��Ķ���
#define BUILTIN_DEFINES_LIST(D) \
    D(include)    /*�ļ�����*/ \
    D(scope) /*�������ֿռ�*/ \
    D(uscp) /*ʹ�����ֿռ�*/ \
    D(delscp) /*ж��ȡ��ʹ�����ֿռ�*/ \
    \
    D(var) /*��������*/ \
    D(set) /*������ֵ*/ \
    D(type)/*���Ͷ���*/ \
    D(tydef)/*����������*/ \
    D(dcl) /*��������*/ \
    D(fun) /*��������*/ \
    D(ret) /*��������*/ \
    \
    D(let) /*���Ű�*/ \
    D(tpf) /*����ģ��*/ \
    D(tpty) /*��ģ��*/ \
    \
    D(adt) /*������ģʽ*/ \
    \
    D(new) /*�ڴ�����*/ \
    D(delete) /*�ڴ��ͷ�*/ \
    D(copy) /*��������*/ \
    \
    D(if) /*if else*/ \
    D(while) /*while*/ \
    \
    D(refer) /*��������*/ \
    D(array) /*��������*/ \
    D(tuple) /*Ԫ������*/ \
    /*D(struct) /*�ṹ����*/ \
    \
    D(arrget) /*�����Ա����*/ \
    D(arrset) /*�����Ա��ֵ*/ \
    /*D(arrrfd) /*�ж����������Ƿ��Ѿ���ֵ����0������Bool*/ \
    \
    D(elmget) /*���Ա����*/ \
    D(elmset) /*���Ա��ֵ*/ \
    /*D(elmrfd) /*�ж����������Ƿ��Ѿ���ֵ����0������Bool*/ \
    D(elmivk) /*���Ա��������*/ \
    D(elmdef) /*���Ա�����ⲿ����*/ \
    \
    D(mcrif)  /*������չ��*/ \
    D(mcrfor) /*���ظ�չ��*/ \
    D(mcrcut) /*��ֶ��п�*/ \
    D(mcrlnk) /*������������*/ \
    \
    D(uvnnew) /*ȫ��Ψһ���ƶ���*/ \
    D(uvnget) /*ȫ��Ψһ���ƻ�ȡ */ \
    D(uvndel) /*ȫ��Ψһ����ɾ��*/ \
    D(uvnclear) /*ȫ��Ψһ�������*/ \
    

    // \
    // D(link) /*ȡ�ñ�������*/ \
    // D(load) /*�������������*/ \
  

// �ڽ���������һ������Ϊ��������ֵ���ͣ�ʣ���Ϊ�������ͣ�
#define BUILTIN_FUNCTION_LIST(B,C) \
    /* �ڽ����� */ \
    B(bool, "Bool,Bool") \
    B(bool, "Bool,Int") \
    /* �����㷨 */ \
    B(add, "Int,Int,Int") \
    B(sub, "Int,Int,Int") \
    B(mul, "Int,Int,Int") \
    /* �����ж� */ \
    B(eq, "Bool,Int,Int") \
    /* C ��׼�⺯�� */ \
    /* math.h */ \
    C(abs, "Int,Int") \
    /* stdio.h */ \
    C(getchar, "Int") \
    C(putchar, "Int,Int") \
    C(getchar, "Char") \
    C(putchar, "Char,Char") \
    C(gets,    "String") \
    C(puts,    "Int,String") \


/*
    B(eq)  /*���* / \
    B(ne)  /*����* / \
    B(gt)  /*����* / \
    B(ge)  /*���ڵ���* / \
    B(lt)  /*С��* / \
    B(le)  /*С�ڵ���* / \
*/