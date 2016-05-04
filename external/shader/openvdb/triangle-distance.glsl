///////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2012-2016 DreamWorks Animation LLC
//
// All rights reserved. This software is distributed under the
// Mozilla Public License 2.0 ( http://www.mozilla.org/MPL/2.0/ )
//
// Redistributions of source code must retain the above copyright
// and license notice and the following restrictions and disclaimer.
//
// *     Neither the name of DreamWorks Animation nor the names of
// its contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// IN NO EVENT SHALL THE COPYRIGHT HOLDERS' AND CONTRIBUTORS' AGGREGATE
// LIABILITY FOR ALL CLAIMS REGARDLESS OF THEIR BASIS EXCEED US$250.00.
//
///////////////////////////////////////////////////////////////////////////

const float epsilon = 1.e-5f;

bool isApproxEqual(in vec3 a, in vec3 b)
{
  return all(lessThan(abs(a-b), vec3(epsilon)));
}



vec3 closestPointOnSegmentToPoint(in vec3 a, in vec3 b, in vec3 p, out float t)
{
    vec3 ab = b - a;
    t = dot(p - a, ab);

    if (t <= 0.0) {
        // c projects outside the [a,b] interval, on the a side.
        t = 0.0;
        return a;
    } else {

        // always nonnegative since denom = ||ab||^2
        float denom = dot(ab, ab);

        if (t >= denom) {
            // c projects outside the [a,b] interval, on the b side.
            t = 1.0;
            return b;
        } else {
            // c projects inside the [a,b] interval.
            t = t / denom;
            return a + (ab * t);
        }
    }
}

vec3
closestPointOnTriangleToPoint(
    in vec3 a, in vec3 b, in vec3 c, in vec3 p, out vec3 uvw)
{
    uvw = vec3(0);

    // degenerate triangle, singular
    if ((isApproxEqual(a, b) && isApproxEqual(a, c))) {
        uvw[0] = 1.0;
        return a;
    }

    vec3 ab = b - a, ac = c - a, ap = p - a;
    float d1 = dot(ab, ap), d2 = dot(ac, ap);

    // degenerate triangle edges
    if (isApproxEqual(a, b)) {

        float t = 0.0;
        vec3 cp = closestPointOnSegmentToPoint(a, c, p, t);

        uvw[0] = 1.0 - t;
        uvw[2] = t;

        return cp;

    } else if (isApproxEqual(a, c) || isApproxEqual(b, c)) {

        float t = 0.0;
        vec3 cp = closestPointOnSegmentToPoint(a, b, p, t);
        uvw[0] = 1.0 - t;
        uvw[1] = t;
        return cp;
    }

    if (d1 <= 0.0 && d2 <= 0.0) {
        uvw[0] = 1.0;
        return a; // barycentric coordinates (1,0,0)
    }

    // Check if P in vertex region outside B
    vec3 bp = p - b;
    float d3 = dot(ab, bp), d4 = dot(ac, bp);
    if (d3 >= 0.0 && d4 <= d3) {
        uvw[1] = 1.0;
        return b; // barycentric coordinates (0,1,0)
    }

    // Check if P in edge region of AB, if so return projection of P onto AB
    float vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0) {
        uvw[1] = d1 / (d1 - d3);
        uvw[0] = 1.0 - uvw[1];
        return a + uvw[1] * ab; // barycentric coordinates (1-v,v,0)
    }

    // Check if P in vertex region outside C
    vec3 cp = p - c;
    float d5 = dot(ab, cp), d6 = dot(ac, cp);
    if (d6 >= 0.0 && d5 <= d6) {
        uvw[2] = 1.0;
        return c; // barycentric coordinates (0,0,1)
    }

    // Check if P in edge region of AC, if so return projection of P onto AC
    float vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0) {
        uvw[2] = d2 / (d2 - d6);
        uvw[0] = 1.0 - uvw[2];
        return a + uvw[2] * ac; // barycentric coordinates (1-w,0,w)
    }

    // Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3*d6 - d5*d4;
    if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0) {
        uvw[2] = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        uvw[1] = 1.0 - uvw[2];
        return b + uvw[2] * (c - b); // barycentric coordinates (0,1-w,w)
    }

    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float denom = 1.0 / (va + vb + vc);
    uvw[2] = vc * denom;
    uvw[1] = vb * denom;
    uvw[0] = 1.0 - uvw[1] - uvw[2];

    return a + ab*uvw[1] + ac*uvw[2]; // = u*a + v*b + w*c , u= va*denom = 1.0-v-w
}


// Copyright (c) 2012-2016 DreamWorks Animation LLC
// All rights reserved. This software is distributed under the
// Mozilla Public License 2.0 ( http://www.mozilla.org/MPL/2.0/ )
