/**
 * @file quad_element_demo.cpp
 * @author your name (you@domain.com)
 * @brief 四边形单元等参变换和数值积分 demo
 * @version 0.1
 * @date 2026-03-31
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <string>

// =====================================================================
// 节点坐标结构
// =====================================================================
struct Node {
    double x, y;
};

// 全局节点坐标数组（供单元访问）
Node nodeCoords[4];

// =====================================================================
// 四边形等参单元
// =====================================================================
struct QuadrilateralElement {
    int nodes[4];  // 四个节点的全局索引

    // -----------------------------------------------------------------
    // 在等参空间中计算形函数值
    //   N0 = 0.25*(1-s)*(1-t)   左下角
    //   N1 = 0.25*(1+s)*(1-t)   右下角
    //   N2 = 0.25*(1+s)*(1+t)   右上角
    //   N3 = 0.25*(1-s)*(1+t)   左上角
    // -----------------------------------------------------------------
    void shapeFunctions(double s, double t, double N[4]) {
        N[0] = 0.25 * (1 - s) * (1 - t);
        N[1] = 0.25 * (1 + s) * (1 - t);
        N[2] = 0.25 * (1 + s) * (1 + t);
        N[3] = 0.25 * (1 - s) * (1 + t);
    }

    // -----------------------------------------------------------------
    // 在等参空间中计算形函数对 s,t 的偏导
    //   dN[i][0] = dN_i/ds
    //   dN[i][1] = dN_i/dt
    // -----------------------------------------------------------------
    void shapeFunctionDerivatives(double s, double t, double dN[4][2]) {
        dN[0][0] = -0.25 * (1 - t);
        dN[0][1] = -0.25 * (1 - s);
        dN[1][0] =  0.25 * (1 - t);
        dN[1][1] = -0.25 * (1 + s);
        dN[2][0] =  0.25 * (1 + t);
        dN[2][1] =  0.25 * (1 + s);
        dN[3][0] = -0.25 * (1 + t);
        dN[3][1] =  0.25 * (1 - s);
    }

    // -----------------------------------------------------------------
    // 计算雅可比矩阵 J = dx/d(s,t)
    //   J = [dx/ds  dx/dt]
    //       [dy/ds  dy/dt]
    // -----------------------------------------------------------------
    void jacobian(double s, double t, double J[2][2]) {
        double dN[4][2];
        shapeFunctionDerivatives(s, t, dN);

        double x[4], y[4];
        for (int i = 0; i < 4; i++) {
            x[i] = nodeCoords[nodes[i]].x;
            y[i] = nodeCoords[nodes[i]].y;
        }

        J[0][0] = J[0][1] = J[1][0] = J[1][1] = 0;
        for (int i = 0; i < 4; i++) {
            J[0][0] += dN[i][0] * x[i];   // dx/ds
            J[0][1] += dN[i][1] * x[i];   // dx/dt
            J[1][0] += dN[i][0] * y[i];   // dy/ds
            J[1][1] += dN[i][1] * y[i];   // dy/dt
        }
    }

    // -----------------------------------------------------------------
    // 计算形函数对物理坐标 x,y 的偏导（通过逆雅可比变换）
    //   [dN/dx]   J^{-1} [dN/ds]
    //   [dN/dy] =        [dN/dt]
    // -----------------------------------------------------------------
    void shapeFunctionDerivativesXY(double s, double t, double dN_xy[4][2]) {
        double dN[4][2];
        shapeFunctionDerivatives(s, t, dN);

        double J[2][2];
        jacobian(s, t, J);

        double detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
        double Jinv[2][2] = {
            { J[1][1]/detJ, -J[0][1]/detJ},
            {-J[1][0]/detJ,  J[0][0]/detJ}
        };

        for (int i = 0; i < 4; i++) {
            dN_xy[i][0] = Jinv[0][0]*dN[i][0] + Jinv[0][1]*dN[i][1];
            dN_xy[i][1] = Jinv[1][0]*dN[i][0] + Jinv[1][1]*dN[i][1];
        }
    }
};

// =====================================================================
// 辅助打印函数
// =====================================================================
void printSeparator(const std::string& title = "") {
    std::cout << "\n" << std::string(60, '=');
    if (!title.empty())
        std::cout << "\n  " << title;
    std::cout << "\n" << std::string(60, '=') << "\n";
}

void printMatrix2x2(const std::string& name, double M[2][2]) {
    std::cout << std::fixed << std::setprecision(6);
    std::cout << name << " =\n";
    std::cout << "  [ " << std::setw(10) << M[0][0]
              << "  " << std::setw(10) << M[0][1] << " ]\n";
    std::cout << "  [ " << std::setw(10) << M[1][0]
              << "  " << std::setw(10) << M[1][1] << " ]\n";
}

// =====================================================================
// 主函数：逐步演示四边形单元的所有计算
// =====================================================================
int main() {
    std::cout << std::string(60, '*') << "\n";
    std::cout << "  四边形等参单元 (Q4) 完整演示程序\n";
    std::cout << std::string(60, '*') << "\n";

    // -----------------------------------------------------------------
    // Step 1: 定义单元节点坐标（一个一般四边形，非矩形）
    // -----------------------------------------------------------------
    printSeparator("Step 1: 定义单元节点坐标");

    // 节点按逆时针排列:
    //  3 (0.1, 1.2) ------- 2 (1.1, 1.0)
    //  |                        |
    //  0 (0.0, 0.0) ------- 1 (1.2, 0.1)
    nodeCoords[0] = {0.0, 0.0};
    nodeCoords[1] = {1.2, 0.1};
    nodeCoords[2] = {1.1, 1.0};
    nodeCoords[3] = {0.1, 1.2};

    std::cout << "节点坐标（逆时针排列）:\n";
    for (int i = 0; i < 4; i++) {
        std::cout << "  Node " << i << ": ("
                  << std::setw(6) << nodeCoords[i].x << ", "
                  << std::setw(6) << nodeCoords[i].y << ")\n";
    }

    // -----------------------------------------------------------------
    // Step 2: 创建单元
    // -----------------------------------------------------------------
    printSeparator("Step 2: 创建四边形单元");

    QuadrilateralElement elem;
    elem.nodes[0] = 0;
    elem.nodes[1] = 1;
    elem.nodes[2] = 2;
    elem.nodes[3] = 3;

    std::cout << "单元节点索引: ["
              << elem.nodes[0] << ", "
              << elem.nodes[1] << ", "
              << elem.nodes[2] << ", "
              << elem.nodes[3] << "]\n";
    std::cout << "等参空间范围: s ∈ [-1, 1],  t ∈ [-1, 1]\n";

    // -----------------------------------------------------------------
    // Step 3: 在单元中心 (s=0, t=0) 计算形函数
    // -----------------------------------------------------------------
    printSeparator("Step 3: 在等参中心 (s=0, t=0) 计算形函数");

    double s = 0.0, t = 0.0;
    double N[4];
    elem.shapeFunctions(s, t, N);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "在 (s=" << s << ", t=" << t << ") 处:\n";
    double sumN = 0;
    for (int i = 0; i < 4; i++) {
        std::cout << "  N" << i << " = 0.25*(1"
                  << (i==0||i==3 ? "-" : "+") << "s)*(1"
                  << (i==0||i==1 ? "-" : "+") << "t)"
                  << " = " << N[i] << "\n";
        sumN += N[i];
    }
    std::cout << "  ∑Ni = " << sumN << "  (应等于 1.0，验证单位分解)\n";

    // 用形函数插值物理坐标（验证）
    double x_interp = 0, y_interp = 0;
    for (int i = 0; i < 4; i++) {
    // (nodeCoords[elem.nodes[i]].x, nodeCoords[elem.nodes[i]].y) 是第 i 个节点的物理坐标
        x_interp += N[i] * nodeCoords[elem.nodes[i]].x;
        y_interp += N[i] * nodeCoords[elem.nodes[i]].y;
    }
    std::cout << "\n等参中心对应的物理坐标（形函数插值）:\n";
    std::cout << "  x = ∑Ni*xi = " << x_interp << "\n";
    std::cout << "  y = ∑Ni*yi = " << y_interp << "\n";

    // -----------------------------------------------------------------
    // Step 4: 计算形函数对 s,t 的偏导
    // -----------------------------------------------------------------
    printSeparator("Step 4: 在等参中心计算形函数偏导 dN/ds, dN/dt");

    double dN[4][2];
    elem.shapeFunctionDerivatives(s, t, dN);

    std::cout << "在 (s=" << s << ", t=" << t << ") 处:\n";
    std::cout << "  i    dNi/ds        dNi/dt\n";
    std::cout << "  " << std::string(35, '-') << "\n";
    for (int i = 0; i < 4; i++) {
        std::cout << "  " << i
                  << "  " << std::setw(12) << dN[i][0]
                  << "  " << std::setw(12) << dN[i][1] << "\n";
    }
    // 验证：∑dNi/ds = 0, ∑dNi/dt = 0
    double sum_ds = 0, sum_dt = 0;
    for (int i = 0; i < 4; i++) { sum_ds += dN[i][0]; sum_dt += dN[i][1]; }
    std::cout << "\n  ∑dNi/ds = " << sum_ds << "  (应为 0)\n";
    std::cout << "  ∑dNi/dt = " << sum_dt << "  (应为 0)\n";

    // -----------------------------------------------------------------
    // Step 5: 计算雅可比矩阵及行列式
    // -----------------------------------------------------------------
    printSeparator("Step 5: 计算雅可比矩阵 J 及其行列式");

    double J[2][2];
    elem.jacobian(s, t, J);

    std::cout << "雅可比矩阵 J = ∂(x,y)/∂(s,t):\n";
    printMatrix2x2("J", J);

    double detJ = J[0][0]*J[1][1] - J[0][1]*J[1][0];
    std::cout << "\ndet(J) = J00*J11 - J01*J10\n";
    std::cout << "       = " << J[0][0] << " * " << J[1][1]
              << " - " << J[0][1] << " * " << J[1][0] << "\n";
    std::cout << "       = " << detJ << "\n";

    if (detJ > 0)
        std::cout << "  det(J) > 0: 单元方向正确（逆时针），映射合法\n";
    else
        std::cout << "  警告: det(J) <= 0，单元可能退化或节点顺序有误！\n";

    // 逆雅可比
    double Jinv[2][2] = {
        { J[1][1]/detJ, -J[0][1]/detJ},
        {-J[1][0]/detJ,  J[0][0]/detJ}
    };
    std::cout << "\n逆雅可比矩阵 J^{-1} = (1/det(J)) * [J11 -J01; -J10 J00]:\n";
    printMatrix2x2("J^{-1}", Jinv);

    // 验证 J * J^{-1} = I
    double I[2][2] = {{0,0},{0,0}};
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++)
                I[i][j] += J[i][k] * Jinv[k][j];
    std::cout << "\n验证 J * J^{-1} (应为单位矩阵):\n";
    printMatrix2x2("J * J^{-1}", I);

    // -----------------------------------------------------------------
    // Step 6: 计算形函数对物理坐标 x,y 的偏导
    // -----------------------------------------------------------------
    printSeparator("Step 6: 计算形函数对物理坐标的偏导 dN/dx, dN/dy");

    double dN_xy[4][2];
    elem.shapeFunctionDerivativesXY(s, t, dN_xy);

    std::cout << "在 (s=" << s << ", t=" << t << ") 处:\n";
    std::cout << "  i    dNi/dx        dNi/dy\n";
    std::cout << "  " << std::string(35, '-') << "\n";
    for (int i = 0; i < 4; i++) {
        std::cout << "  " << i
                  << "  " << std::setw(12) << dN_xy[i][0]
                  << "  " << std::setw(12) << dN_xy[i][1] << "\n";
    }

    // -----------------------------------------------------------------
    // Step 7: 高斯积分点处的完整计算
    // -----------------------------------------------------------------
    printSeparator("Step 7: 2×2 高斯积分（模拟单元刚度矩阵组装）");

    // 2×2 高斯点: ±1/√3
    const double gp = 1.0 / std::sqrt(3.0);
    double gauss_pts[2] = {-gp, gp};
    double gauss_wts[2] = { 1.0, 1.0};

    std::cout << "高斯积分点坐标: ±1/√3 ≈ ±" << std::fixed << std::setprecision(6) << gp << "\n";
    std::cout << "权重: 1.0\n\n";

    // 模拟 Laplacian 单元刚度矩阵 K_e = ∫ B^T B dΩ
    // 其中 B = [dN/dx; dN/dy]（4列，每列对应一个节点）
    double Ke[4][4] = {};

    std::cout << std::left << std::setw(6) << "GP#"
              << std::setw(12) << "s"
              << std::setw(12) << "t"
              << std::setw(12) << "det(J)"
              << "  贡献到 K_e\n";
    std::cout << std::string(55, '-') << "\n";

    int gp_count = 0;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            double sg = gauss_pts[i];
            double tg = gauss_pts[j];
            double w  = gauss_wts[i] * gauss_wts[j];

            double Jg[2][2];
            elem.jacobian(sg, tg, Jg);
            double detJg = Jg[0][0]*Jg[1][1] - Jg[0][1]*Jg[1][0];

            double B[4][2];
            elem.shapeFunctionDerivativesXY(sg, tg, B);

            // K_e += w * B^T * B * det(J)
            for (int a = 0; a < 4; a++)
                for (int b = 0; b < 4; b++)
                    Ke[a][b] += w * (B[a][0]*B[b][0] + B[a][1]*B[b][1]) * detJg;

            std::cout << std::right << std::setw(4) << ++gp_count
                      << std::fixed << std::setprecision(6)
                      << "  " << std::setw(10) << sg
                      << "  " << std::setw(10) << tg
                      << "  " << std::setw(10) << detJg
                      << "  ✓\n";
        }
    }

    std::cout << "\n单元刚度矩阵 K_e (Laplacian, 4×4):\n";
    std::cout << std::fixed << std::setprecision(5);
    for (int i = 0; i < 4; i++) {
        std::cout << "  [";
        for (int j = 0; j < 4; j++) {
            std::cout << std::setw(10) << Ke[i][j];
            if (j < 3) std::cout << "  ";
        }
        std::cout << " ]\n";
    }

    // 验证：K_e 应为对称阵，行和应约为零（对均匀问题）
    std::cout << "\n对称性验证 (K_e[i][j] - K_e[j][i]):\n";
    double max_asym = 0;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            max_asym = std::max(max_asym, std::abs(Ke[i][j] - Ke[j][i]));
    std::cout << "  最大非对称误差 = " << max_asym << "  (应为 0)\n";

    std::cout << "\n行和验证 (∑_j K_e[i][j]):\n";
    for (int i = 0; i < 4; i++) {
        double row_sum = 0;
        for (int j = 0; j < 4; j++) row_sum += Ke[i][j];
        std::cout << "  行 " << i << " 行和 = " << row_sum
                  << "  (Neumann 齐次边界下应≈0)\n";
    }

    // -----------------------------------------------------------------
    // Step 8: 任意点的坐标映射验证
    // -----------------------------------------------------------------
    printSeparator("Step 8: 验证等参映射（角点等参坐标应映射回节点）");

    double corner_st[4][2] = {{-1,-1},{1,-1},{1,1},{-1,1}};
    std::cout << std::fixed << std::setprecision(4);
    std::cout << std::left
              << std::setw(6)  << "节点"
              << std::setw(8)  << "s"
              << std::setw(8)  << "t"
              << std::setw(12) << "x(插值)"
              << std::setw(12) << "y(插值)"
              << std::setw(12) << "x(实际)"
              << std::setw(12) << "y(实际)" << "\n";
    std::cout << std::string(58, '-') << "\n";

    for (int k = 0; k < 4; k++) {
        double sk = corner_st[k][0], tk = corner_st[k][1];
        double Nk[4];
        elem.shapeFunctions(sk, tk, Nk);
        double xi = 0, yi = 0;
        for (int i = 0; i < 4; i++) {
            xi += Nk[i] * nodeCoords[elem.nodes[i]].x;
            yi += Nk[i] * nodeCoords[elem.nodes[i]].y;
        }
        std::cout << std::right
                  << std::setw(4)  << k
                  << std::setw(8)  << sk
                  << std::setw(8)  << tk
                  << std::setw(12) << xi
                  << std::setw(12) << yi
                  << std::setw(12) << nodeCoords[k].x
                  << std::setw(12) << nodeCoords[k].y
                  << "\n";
    }

    // -----------------------------------------------------------------
    // 完成
    // -----------------------------------------------------------------
    std::cout << "\n" << std::string(60, '*') << "\n";
    std::cout << "  演示完成\n";
    std::cout << std::string(60, '*') << "\n\n";

    return 0;
}