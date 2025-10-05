struct Complex
{
    double real;
    double imag;
};

Complex operator+(Complex a, Complex b)
{
    Complex result;
    result.real = a.real + b.real;
    result.imag = a.imag + b.imag;
    return result;
}

Complex operator-(Complex a, Complex b)
{
    Complex result;
    result.real = a.real - b.real;
    result.imag = a.imag - b.imag;
    return result;
}

Complex operator*(Complex a, Complex b)
{
    Complex result;
    result.real = a.real * b.real - a.imag * b.imag;
    result.imag = a.real * b.imag + a.imag * b.real;
    return result;
}

Complex operator/(Complex a, Complex b)
{
    Complex result;
    double den = b.real * b.real + b.imag * b.imag;
    if (den < 1e-10)    // do not divide by numbers close to 0
    {
        result.real = 0.0;
        result.imag = 0.0;
        return result;
    }
    result.real = (a.real * b.real + a.imag * b.imag) / den;
    result.imag = (a.imag * b.real - a.real * b.imag) / den;
    return result;
}

Complex operator+=(Complex& a, Complex b)
{
    a.real += b.real;
    a.imag += b.imag;
    return a;
}

Complex operator-=(Complex& a, Complex b)
{
    a.real -= b.real;
    a.imag -= b.imag;
    return a;
}

Complex pow(Complex z, int n)
{
    Complex result = {1.0, 0.0};
    if (n == 0)
    {
        return result;
    }

    // do not support negative n
    if (n < 0)
    {
        result.real = 0.0;
        result.imag = 0.0;
        return result;
    }

    for (uniform int i = 0; i < n; ++i)
    {
        result = result * z;
    }

    return result;
}

double abs(Complex z)
{
    return sqrt(z.real * z.real + z.imag * z.imag);
}

/// get value of a polynomial like z^n - 1 = 0
/// @param n power of z
/// @param z value of z
Complex getNormal(uniform int n, Complex z)
{
    uniform Complex one = {1.0, 0.0};
    return pow(z, n) - one;
}

/// get derivative of a polynomial like z^n - 1 = 0.
/// @param n power of z
/// @param z value of z
Complex getDerivative(uniform int n, Complex z)
{
    uniform Complex nComp = {(double)n, 0.0};
    return nComp * pow(z, n - 1);
}

uniform double pi = 3.141592653589793;

void findRoots(uniform int n, uniform Complex* uniform roots)
{
    for (uniform int k = 0; k < n; ++k)
    {
        uniform double angle = 2.0 * pi * k / n;
        roots[k].real = cos(angle);
        roots[k].imag = sin(angle);
    }
}

export void computeNewtonFractal(
    uniform int n, uniform int width, uniform int height,
    uniform double xmin, uniform double xmax,
    uniform double ymin, uniform double ymax,
    uniform int maxIter,
    uniform double tolerance,
    uniform int rootIdx[],
    uniform int itCounts[]
)
{
    uniform double xstep = (xmax - xmin) / width;
    uniform double ystep = (ymax - ymin) / height;

    uniform Complex* uniform roots = uniform new Complex[n];
    findRoots(n, roots);

    for (int row = 0; row < height; ++row)
    {
        foreach(col = 0 ... width)
        {
            double curx = xmin + (col * xstep);
            double cury = ymin + (row * ystep);

            Complex point = {curx, cury};
            int iter = 0;

            // find the nearest root using Euler's method
            while (iter < maxIter)
            {
                Complex normal = getNormal(n, point);
                Complex derivative = getDerivative(n, point);

                if (abs(normal) < tolerance || abs(derivative) < tolerance)
                {
                    break;
                }

                Complex step = normal / derivative;
                point -= step;
                ++iter;
            }

            // determine root index
            int rootIndex = -1; // default: no root

            double minDist = DBL_MAX;
            for (int i = 0; i < n; ++i)
            {
                double rootDist = abs(point - roots[i]);
                if (rootDist > tolerance)
                {
                    continue;
                }
                if (rootDist < minDist)
                {
                    minDist = rootDist;
                    rootIndex = i;
                }
            }

            int idx = row * width + col;
            rootIdx[idx] = rootIndex;
            itCounts[idx] = iter;
        }
    }

    delete[] roots;
}