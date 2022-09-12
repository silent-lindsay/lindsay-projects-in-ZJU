class A {
    int f(int k) {
        return k*2;
    }    
}

public class B {
    public static void main(String[] args) {
        A a = new A();
        System.out.println(a.f(10));
    }
}