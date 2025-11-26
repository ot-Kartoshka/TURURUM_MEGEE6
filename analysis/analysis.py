# -*- coding: utf-8 -*-
import pandas as pd
import matplotlib.pyplot as plt
import os
import argparse

def make_safe(name: str):
    return name.lower().replace(" ", "_").replace("/", "")


def create_plots_for_metric(df, metric, outdir, csv_tag):

    
    kinds = df["kind"].unique()
    variants = df["variant"].unique()
    
    print(f"  > Plotting by 'kind' for metric: {metric}")
    for kind in kinds:
        plt.figure(figsize=(10, 6))
        subset = df[df["kind"] == kind]
        
        for variant in variants:
            variant_data = subset[subset["variant"] == variant]
            if variant_data.empty:
                continue
            
            grouped = variant_data.groupby("size")[metric].mean()
            plt.plot(grouped.index, grouped.values, marker="o", label=variant)

        safe_kind = make_safe(kind)
        safe_metric = make_safe(metric)
        outname = os.path.join(outdir, f"{safe_metric}__{safe_kind}_{csv_tag}.png")
        
        plt.xlabel("Input size (n)")
        plt.ylabel(metric)
        plt.title(f"{metric} vs. Size (Kind: {kind})")
        plt.legend()
        plt.grid(True)
        plt.savefig(outname)
        print(f"    [OK] Saved {outname}")
        plt.close()
    
    plt.figure(figsize=(10, 6))
    
    for variant in variants:
        variant_data = df[df["variant"] == variant]
        if variant_data.empty:
            continue

        grouped = variant_data.groupby("size")[metric].mean()
        
        plt.plot(grouped.index, grouped.values, marker="o", label=variant)

    safe_metric = make_safe(metric)
    outname_all = os.path.join(outdir, f"{safe_metric}__OVERALL_AVERAGE_{csv_tag}.png")
    
    plt.xlabel("Input size (n)")
    plt.ylabel(f"Average {metric}")
    plt.title(f"Overall Average {metric} vs. Size (All Kinds)")
    plt.legend()
    plt.grid(True)
    plt.savefig(outname_all)
    print(f"    [OK] Saved {outname_all}")
    plt.close()


def main():
    parser = argparse.ArgumentParser(description="Plot merge-sort benchmark results")
    parser.add_argument("--input", required=True, help="Path to CSV results file")
    args = parser.parse_args()
    csv_path = args.input

    outdir = "benchmarks/plots"
    os.makedirs(outdir, exist_ok=True)

    if not os.path.exists(csv_path):
        print(f"[ERROR] File not found: {csv_path}")
        return

    print(f"[INFO] Loading CSV: {csv_path}")
    df = pd.read_csv(csv_path)

    csv_filename = os.path.basename(csv_path)
    csv_tag = os.path.splitext(csv_filename)[0]

    df_regular = df[df["kind"] != "gensort"]

    if df_regular.empty:
        print("[WARNING] CSV contains only 'gensort' data. Nothing to plot.")
        return

    metrics = ["elapsed_ms", "comparisons", "copies", "aux_bytes"]

    for metric in metrics:
        print(f"\n[INFO] Processing metric: {metric}")
        create_plots_for_metric(df_regular, metric, outdir, csv_tag)

    print(f"\n[DONE] All plots saved to {outdir}")


if __name__ == "__main__":
    main()