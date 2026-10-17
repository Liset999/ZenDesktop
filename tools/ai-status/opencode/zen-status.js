// ZenDesktop taskbar AI status plugin for opencode.
// Copy to ~/.config/opencode/plugins/ (global) or <project>/.opencode/plugins/.
// Writes %LOCALAPPDATA%\ZenDesktop\ai-status\opencode.json (temp file + rename), read by the
// "ZenDesktop: Taskbar Music & Lyrics" Windhawk mod. Never throws into opencode.
import fs from "node:fs"
import os from "node:os"
import path from "node:path"

const DIR = path.join(process.env.LOCALAPPDATA || path.join(os.homedir(), "AppData", "Local"), "ZenDesktop", "ai-status")
const FILE = path.join(DIR, "opencode.json")

function write(state, { detail = "", project, clearMessage = false } = {}) {
  try {
    fs.mkdirSync(DIR, { recursive: true })
    let old = {}
    try { old = JSON.parse(fs.readFileSync(FILE, "utf8")) } catch {}
    const now = Math.floor(Date.now() / 1000)
    // A late tool event must not undo a just-written done/error.
    if ((state === "tool" || state === "working") && ["done", "error"].includes(old.state) && now - (old.updated || 0) <= 2) return
    const data = {
      agent: "opencode",
      name: "opencode",
      state,
      message: clearMessage ? "" : (old.message || ""),
      detail: String(detail || "").slice(0, 60),
      project: project ?? old.project ?? "",
      updated: now,
    }
    const tmp = `${FILE}.${process.pid}.tmp`
    fs.writeFileSync(tmp, JSON.stringify(data), "utf8")
    fs.renameSync(tmp, FILE)
  } catch {}
}

export const ZenStatus = async ({ directory }) => {
  const project = path.basename(directory || "")
  write("idle", { project, clearMessage: true })
  return {
    event: async ({ event }) => {
      switch (event?.type) {
        case "session.status": {
          const t = event.properties?.status?.type
          if (t === "busy") write("working", { project })
          else if (t === "retry") write("working", { project, detail: "retry" })
          break
        }
        case "session.idle":
          write("done", { project })
          break
        case "session.error":
          write("error", { project })
          break
        case "permission.asked":
        case "permission.updated":
          write("waiting", { project })
          break
        case "permission.replied":
          write("working", { project })
          break
      }
    },
    "tool.execute.before": async (input) => {
      if (String(input?.tool || "").startsWith("zen-status")) return
      write("tool", { project, detail: input?.tool })
    },
    "tool.execute.after": async () => write("working", { project }),
  }
}
