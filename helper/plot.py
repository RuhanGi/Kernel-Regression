import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

def plot_gp_assessment(csv_path):
    # Load master results
    try:
        df = pd.read_csv(csv_path)
    except FileNotFoundError:
        print(f"Error: {csv_path} not found. Run the C++ assessment first.")
        return

    # Create a 2x2 dashboard
    fig = plt.figure(figsize=(18, 12))
    plt.suptitle("HW4: Gaussian Process Regression Assessment", fontsize=20)

    # Filter data types
    history_df = df[df['DataType'] == 'History']
    spatial_df = df[df['DataType'] == 'Spatial']

    # --- Panel 1: MSE Convergence (All Kernels) ---
    ax1 = fig.add_subplot(221)
    for kernel in history_df['Kernel'].unique():
        k_data = history_df[history_df['Kernel'] == kernel]
        ax1.plot(k_data['Epoch'], k_data['MSE'], label=f'{kernel} MSE', linewidth=2)
    ax1.set_title("MSE Convergence Comparison")
    ax1.set_xlabel("Epoch")
    ax1.set_ylabel("Mean Squared Error")
    ax1.legend()
    ax1.grid(True, alpha=0.3)

    # --- Panel 2: R^2 Convergence (All Kernels) ---
    ax2 = fig.add_subplot(222)
    for kernel in history_df['Kernel'].unique():
        k_data = history_df[history_df['Kernel'] == kernel]
        ax2.plot(k_data['Epoch'], k_data['R2'], label=f'{kernel} $R^2$', linewidth=2)
    ax2.set_title("$R^2$ Performance Comparison")
    ax2.set_xlabel("Epoch")
    ax2.set_ylabel("$R^2$ Score")
    ax2.legend()
    ax2.grid(True, alpha=0.3)

    best_k = history_df.sort_values('R2', ascending=False).iloc[0]['Kernel']
    k_spatial = spatial_df[spatial_df['Kernel'] == best_k]
    
    # CRITICAL FIX: Sort by X1 here so it is available for all following panels
    k_sorted = k_spatial.sort_values('X1')

    # --- Panel 3: 3D Surface / 2D Spatial Map ---
    ax3 = fig.add_subplot(223, projection='3d')
    
    # Check if X2 has variance (2D dataset check)
    if k_spatial['X2'].nunique() > 1:
        try:
            ax3.plot_trisurf(k_spatial['X1'], k_spatial['X2'], k_spatial['Mean'], 
                            cmap='viridis', alpha=0.8, edgecolor='none')
            ax3.set_title(f"3D Predictive Surface ({best_k})")
        except RuntimeError:
            ax3.scatter(k_spatial['X1'], k_spatial['X2'], k_spatial['Mean'], c='tab:red')
            ax3.set_title("3D Scatter (Singular Geometry)")
    else:
        # Fallback for 1D datasets (e.g., Automobile, Forest Fires) [cite: 20, 39]
        ax3.view_init(elev=20., azim=-35)
        ax3.scatter(k_sorted['X1'], [0]*len(k_sorted), k_sorted['TrueY'], 
                   color='black', s=10, label='Ground Truth', alpha=0.4)
        ax3.plot(k_sorted['X1'], [0]*len(k_sorted), k_sorted['Mean'], 
                 color='tab:red', linewidth=2, label='GP Prediction')
        ax3.set_title(f"1D Projection in 3D Space ({best_k})")
        ax3.set_ylabel("Constant X2")

    ax3.set_xlabel("Feature X1")
    ax3.set_zlabel("Target Value")

    # --- Panel 4: 1D Prediction & 95% Confidence Interval ---
    ax4 = fig.add_subplot(224)
    # Calculate 95% Confidence Interval: 1.96 * standard deviation [cite: 18, 49]
    ci = 1.96 * k_sorted['Std']
    
    ax4.scatter(k_sorted['X1'], k_sorted['TrueY'], color='black', s=10, alpha=0.5, label='True Data')
    ax4.plot(k_sorted['X1'], k_sorted['Mean'], color='tab:red', label='GP Mean')
    ax4.fill_between(k_sorted['X1'], 
                     k_sorted['Mean'] - ci, 
                     k_sorted['Mean'] + ci, 
                     color='tab:red', alpha=0.2, label='95% Confidence Interval')
    
    ax4.set_title(f"Prediction Fit vs X1 ({best_k})")
    ax4.set_xlabel("Input Feature (X1)")
    ax4.set_ylabel("Target Value")
    ax4.legend()

    plt.tight_layout(rect=[0, 0.03, 1, 0.95])
    plt.savefig("gp_assessment_dashboard.png", dpi=300)
    print("Dashboard saved as gp_assessment_dashboard.png")
    plt.show()

if __name__ == "__main__":
    plot_gp_assessment("results.csv")