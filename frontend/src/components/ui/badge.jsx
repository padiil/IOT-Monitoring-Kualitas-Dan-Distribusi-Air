import * as React from "react"
import { cva } from "class-variance-authority";

import { cn } from "@/lib/utils"

const badgeVariants = cva(
  "inline-flex items-center rounded-md border px-2.5 py-0.5 text-xs font-semibold transition-colors focus:outline-none focus:ring-2 focus:ring-offset-2",
  {
    variants: {
      variant: {
        default:
          "border-transparent bg-neutral-900 text-neutral-50 shadow hover:bg-neutral-900/80 dark:bg-neutral-50 dark:text-neutral-900 dark:hover:bg-neutral-50/80",
        secondary:
          "border-transparent bg-neutral-100 text-neutral-50 hover:bg-neutral-100/80 dark:bg-neutral-800 dark:text-neutral-50 dark:hover:bg-neutral-800/80",
        success:
          "border-transparent bg-green-400 text-neutral-50 shadow hover:bg-green-400/80 dark:bg-green-600 dark:text-neutral-50 dark:hover:bg-green-600/80",
        warning:
          "border-transparent bg-yellow-400 text-neutral-50 shadow hover:bg-yellow-400/80 dark:bg-yellow-600 dark:text-neutral-50 dark:hover:bg-yellow-600/80",
        mediumWarning:
          "border-transparent bg-orange-400 text-neutral-50 shadow hover:bg-orange-400/80 dark:bg-orange-600 dark:text-neutral-50 dark:hover:bg-orange-600/80",
        destructive:
          "border-transparent bg-red-600 text-neutral-50 shadow hover:bg-red-600/80 dark:bg-red-800 dark:text-neutral-50 dark:hover:bg-red-800/80",
        outline: "text-neutral-950 dark:text-neutral-50",
      },
    },
    defaultVariants: {
      variant: "default",
    },
  }
);


function Badge({
  className,
  variant,
  ...props
}) {
  return (<div className={cn(badgeVariants({ variant }), className)} {...props} />);
}

export { Badge, badgeVariants }
